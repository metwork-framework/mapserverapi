#include "mapserverapi.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>

#include "random.h"
#include "mapserver.h"

static gboolean __MAPSERVERAPI_INITIALIZED = FALSE;
static gchar *__MAPSERVERAPI_TMPDIR = NULL;
static gboolean __MAPSERVER_SETUP = FALSE;

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
    }
    tmp = g_getenv("TMPDIR");
    if (tmp != NULL) {
        __MAPSERVERAPI_TMPDIR = g_strdup(tmp);
    }
    tmp = g_getenv("TMP");
    if (tmp != NULL) {
        __MAPSERVERAPI_TMPDIR = g_strdup(tmp);
    }
    __MAPSERVERAPI_TMPDIR = g_strdup("/tmp");
    if (__MAPSERVER_SETUP == FALSE) {
        msSetup();
        __MAPSERVER_SETUP = TRUE;
    }
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
    gsize i;
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
        gs = g_string_append(gs, query_string + 1);
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
    unlink(mapfile_path);
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
    gsize last_i = 0;
    gchar *ct = NULL;
    const char *mb = (char*) mapserver_buffer;
    for (i = 0 ; i < mapserver_buffer_length - 1 ; i++) {
        if (i > 100) {
            break;
        }
        if ((mb[i] == '\r') && (mb[i+1] == '\n')) {
            if (i == last_i) {
                last_i = i + 2;
                break;
            }
            if (strncmp(mb + last_i, "Content-Type: ", 14) == 0) {
                ct = g_malloc(i - last_i - 14 + 1);
                ct[i - last_i - 14] = '\0';
                memcpy(ct, mapserver_buffer + (last_i + 14), i - last_i - 14);
            }
            last_i = i + 2;
        }
    }
    if (i >= mapserver_buffer_length) {
        g_message("bad reply from mapserver for query_string = %s (no body found)", mapserver_qs);
        g_free(mapserver_qs);
        return FALSE;
    }
    if (ct == NULL) {
        g_message("bad reply from mapserver for query_string = %s (no Content-Type found)", mapserver_qs);
        g_free(mapserver_qs);
        return FALSE;
    }
    if (content_type != NULL) {
        *content_type = ct;
    } else {
        g_free(ct);
    }
    if (body != NULL) {
        *body = mapserver_buffer + last_i;
    }
    if (body_length != NULL) {
        *body_length = mapserver_buffer_length - last_i;
    }
    g_free(mapserver_qs);
    return res;
}

gchar *mapserverapi_invoke_to_file(const gchar *mapfile_content, const gchar *query_string,
        const gchar *target_file) {
    void *body = NULL;
    gsize body_length;
    gchar *tmp_content_type = NULL;
    gboolean res = mapserverapi_invoke(mapfile_content, query_string, &body,
            &tmp_content_type, &body_length);
    if (res == FALSE) {
        return NULL;
    }
    GError *error = NULL;
    gboolean res2 = g_file_set_contents(target_file, body, body_length, &error);
    if (res2 == FALSE) {
        g_warning("can't write mapserver output to target_file: %s", target_file);
        if (error != NULL) {
            g_warning("error message: %s", error->message);
        }
        g_free(tmp_content_type);
        return NULL;
    }
    return tmp_content_type;
}
