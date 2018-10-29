#include <glib.h>
#include <glib-object.h>
#include <glib/gtestutils.h>
#include <unistd.h>
#include "mapserverapi.h"

static gboolean integration = FALSE;
static gchar *directory = NULL;

void test_mapserverapi_newfree()
{
    MapServerApiHandle *msah = mapserverapi_new();
    mapserverapi_free(msah);
}

void test_mapserverapi_get_response()
{
    MapServerApiHandle *msah = mapserverapi_new();
    gchar *contents;
    gchar *path = g_strdup_printf("%s/naturalearth.map", directory);
    g_file_get_contents(path, &contents, NULL, NULL);
    mapserverapi_set_mapfile_content(msah, contents);
    mapserverapi_set_query_string_from_query_string(msah, "LAYERS=admin_0_countries&TRANSPARENT=true&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_inimage&SRS=EPSG%3A4326&BBOX=-180.0,-90.0,180.0,90.0&WIDTH=500&HEIGHT=250");
    g_free(path);
    g_free(contents);
    void *body;
    gchar *content_type = NULL;
    gsize body_length;
    gboolean b = mapserverapi_get_response(msah, &body, &content_type, &body_length);
    g_assert(b == TRUE);
    g_assert_cmpstr(content_type, ==, "image/png");
    g_assert_cmpint(body_length, >, 1000);
    g_free(content_type);
    g_file_set_contents("/tmp/bodyout.png", body, body_length, NULL);
    unlink("/tmp/bodyout.png");
    mapserverapi_free(msah);
}

int main(int argc, char** argv)
{
    g_test_init (&argc, &argv, NULL);
    if ((argc >= 2) && (g_strcmp0(argv[1], "INTEGRATION") == 0)) integration = TRUE;
    directory = g_strdup("../test");
    g_type_init();
    g_thread_init(NULL);
    g_test_add_func("/mapserverapi/new_free", test_mapserverapi_newfree);
    if (integration) {
        g_test_add_func("/mapserverapi/get_response", test_mapserverapi_get_response);
    }
    gint res = g_test_run();
    g_free(directory);
    return res;
}
