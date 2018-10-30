#include "mapserverapi.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>

#include "random.h"
#include "mapserver.h"

static gboolean __MAPSERVERAPI_INITIALIZED = FALSE;
static gchar *__MAPSERVERAPI_TMPDIR = NULL;

void mapserverapi_init() {
    if (__MAPSERVERAPI_INITIALIZED) {
        g_critical("mapserverapi already initialized: call mapserverapi_destroy() before");
        g_assert(__MAPSERVERAPI_INITIALIZED == FALSE);
    }
    __MAPSERVERAPI_INITIALIZED = TRUE;
    const gchar *tmp;
    tmp = g_getenv("MAPSERVERAPI_TMPDIR");
    if (tmp != NULL) {
        __MAPSERVERAPI_TMPDIR = g_strdup(tmp);
        return;
    }
    tmp = g_getenv("TMPDIR");
    if (tmp != NULL) {
        __MAPSERVERAPI_TMPDIR = g_strdup(tmp);
        return;
    }
    tmp = g_getenv("TMP");
    if (tmp != NULL) {
        __MAPSERVERAPI_TMPDIR = g_strdup(tmp);
        return;
    }
    __MAPSERVERAPI_TMPDIR = g_strdup("/tmp");
}

void mapserverapi_destroy() {
    if (__MAPSERVERAPI_INITIALIZED == FALSE) {
        g_critical("mapserverapi not initialized: call mapserverapi_init() before");
        g_assert(__MAPSERVERAPI_INITIALIZED == TRUE);
    }
    msIO_resetHandlers();
    __MAPSERVERAPI_INITIALIZED = FALSE;
    g_free(__MAPSERVERAPI_TMPDIR);
}

gchar *get_tmpfilename() {
    gchar *unique = get_unique_hexa_identifier();
    gchar *tmp = g_strdup_printf("%s/mapserverapi_%s.map", __MAPSERVERAPI_TMPDIR,
            unique);
    g_free(unique);
    return tmp;
}

void assert_mapserverapi_initialized() {
    if (__MAPSERVERAPI_INITIALIZED == FALSE) {
        g_critical("mapserverapi not initialized: call mapserverapi_init() before use");
        g_assert(__MAPSERVERAPI_INITIALIZED);
    }
}

gboolean mapserverapi_invoke(const gchar *mapfile_content, const gchar *query_string, void **body,
    gchar **content_type, gsize *body_length) {
    assert_mapserverapi_initialized();
    g_assert(mapfile_content != NULL);
    g_assert(body != NULL);
    g_assert(query_string != NULL);
    g_assert(strlen(query_string) > 0);
    gboolean res;
    gchar *mapfile_path = get_tmpfilename();
    res = g_file_set_contents(mapfile_path, mapfile_content, -1, NULL);
    if (!res) {
        g_critical("can't write mapfile in %s", mapfile_path);
        g_free(mapfile_path);
        return FALSE;
    }
    GString *gs = g_string_new(NULL);
    g_string_append_printf(gs, "MAP=%s", mapfile_path);
    gs = g_string_append_c(gs, '&');
    if (query_string[0] == '?') {
        gs = g_string_append(gs, query_string + sizeof(gchar));
    } else {
        gs = g_string_append(gs, query_string);
    }
    gchar *mapserver_qs = g_string_free(gs, FALSE);
    int mapserver_status;
    void *mapserver_buffer;
    size_t mapserver_buffer_length;
    res = TRUE;
    g_debug("Calling mapserver with query_string[%s]...", mapserver_qs);
    mapserver_status = msCGIHandler(mapserver_qs, &mapserver_buffer, &mapserver_buffer_length);
    //unlink(mapfile_path);
    g_free(mapfile_path);
    if (mapserver_status != 0) {
        g_warning("bad reply from mapserver for query_string = %s", mapserver_qs);
        res = FALSE;
    }
    if (mapserver_buffer_length < 14) {
        g_warning("bad reply from mapserver for query_string = %s (no Content-Type)", mapserver_qs);
        g_free(mapserver_qs);
        return FALSE;
    }
    if (strncasecmp((const char*) mapserver_buffer, "Content-type: ", 14) != 0) {
        g_warning("bad reply from mapserver for query_string = %s (no Content-Type)", mapserver_qs);
        g_free(mapserver_qs);
        return FALSE;
    }
    int end_of_ct = 13;
    while ((end_of_ct + 1 < mapserver_buffer_length) && (((char*)mapserver_buffer)[end_of_ct + 1] != 10)) {
        end_of_ct++;
    }
    if (end_of_ct + 1 == mapserver_buffer_length) {
        g_warning("bad reply from mapserver for query_string = %s (bad Content-Type)", query_string);
        g_free(mapserver_qs);
        return FALSE;
    }
    int start_of_data = end_of_ct + 2;
    while ((start_of_data < mapserver_buffer_length) && (((char*)mapserver_buffer)[start_of_data] != 10)) {
        start_of_data++;
    }
    if (start_of_data == mapserver_buffer_length) {
        g_warning("bad reply from mapserver for query_string = %s (corrupt Content-Type)", mapserver_qs);
        g_free(mapserver_qs);
        return FALSE;
    }
    start_of_data++;
    gchar *ct = g_malloc((end_of_ct - 14 + 2) * sizeof(gchar));
    memcpy(ct, mapserver_buffer + 14 * sizeof(gchar), sizeof(gchar) * (end_of_ct - 14 + 2));
    ct[end_of_ct - 14 + 1] = '\0';
    if (content_type != NULL) {
        *content_type = g_strstrip(ct);
    } else {
        g_free(ct);
    }
    if (body != NULL) {
        *body = mapserver_buffer + start_of_data * sizeof(gchar);
    }
    if (body_length != NULL) {
        *body_length = mapserver_buffer_length - start_of_data;
    }
    g_free(mapserver_qs);
    return res;
}
