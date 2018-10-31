#include <glib.h>
#include <glib-object.h>
#include <glib/gtestutils.h>
#include <limits.h>
#include <stdlib.h>
#include "mapserverapi.h"

gchar *get_data_path()
{
    gchar *res = NULL;
    char *rp = realpath("../test_datas", NULL);
    res = g_strdup(rp);
    free(rp);
    return res;
}

gchar *get_mapfile_content()
{
    gchar *contents;
    gchar *data_path = get_data_path();
    gchar *path = g_strdup_printf("%s/test.map", data_path);
    g_file_get_contents(path, &contents, NULL, NULL);
    g_free(path);
    gchar **split = g_strsplit((const gchar*) contents, "{DATAPATH}", -1);
    g_free(contents);
    contents = g_strjoinv(data_path, split);
    g_free(data_path);
    g_strfreev(split);
    return contents;
}

void test_mapserverapi_init()
{
    mapserverapi_init();
    mapserverapi_destroy();
}

void test_mapserverapi_invoke()
{
    mapserverapi_init();
    gchar *contents = get_mapfile_content();
    void *body;
    gchar *content_type = NULL;
    gsize body_length;
    gboolean b = mapserverapi_invoke(contents, "LAYERS=ocean&TRANSPARENT=true&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_xml&SRS=EPSG%3A4326&BBOX=-180.0,-90.0,180.0,90.0&WIDTH=500&HEIGHT=250", &body, &content_type, &body_length);
    g_free(contents);
    g_assert(b == TRUE);
    g_file_set_contents("./testresult.png", body, body_length, NULL);
    g_assert_cmpint(body_length, >, 1000);
    g_assert_cmpstr(content_type, ==, "image/png");
    g_free(content_type);
    mapserverapi_destroy();
}

void test_mapserverapi_invoke_to_file()
{
    mapserverapi_init();
    gchar *contents = get_mapfile_content();
    gchar *content_type = mapserverapi_invoke_to_file(contents, "LAYERS=ocean&TRANSPARENT=true&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_xml&SRS=EPSG%3A4326&BBOX=-180.0,-90.0,180.0,90.0&WIDTH=500&HEIGHT=250", "/tmp/test_mapserverapi_invoke_to_file");
    g_free(contents);
    g_assert(content_type != NULL);
    g_assert_cmpstr(content_type, ==, "image/png");
    gsize body_length;
    gchar *body;
    gboolean res = g_file_get_contents("/tmp/test_mapserverapi_invoke_to_file", &body, &body_length, NULL);
    g_assert(res == TRUE);
    g_assert_cmpint(body_length, >, 1000);
    g_free(body);
    g_free(content_type);
    mapserverapi_destroy();
}

int main(int argc, char** argv)
{
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/mapserverapi/new_free", test_mapserverapi_init);
    g_test_add_func("/mapserverapi/invoke", test_mapserverapi_invoke);
    g_test_add_func("/mapserverapi/invoke_to_file", test_mapserverapi_invoke_to_file);
    gint res = g_test_run();
    return res;
}
