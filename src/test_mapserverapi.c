#include <glib.h>
#include <glib-object.h>
#include <glib/gtestutils.h>
#include "mapserverapi.h"

static gchar *directory = "../test_datas";

gchar *get_data_path()
{
    gchar *tmp = g_get_current_dir();
    gchar *res = g_strdup_printf("%s/../test_datas", tmp);
    g_free(tmp);
    return res;
}

void test_mapserverapi_init()
{
    mapserverapi_init();
    mapserverapi_destroy();
}

void test_mapserverapi_invoke()
{
    mapserverapi_init();
    gchar *contents;
    void *body;
    gchar *content_type = NULL;
    gsize body_length;
    gchar *path = g_strdup_printf("%s/test.map", directory);
    g_file_get_contents(path, &contents, NULL, NULL);
    gchar *datapath = get_data_path();
    g_debug("path = %s", path);
    g_debug("datapath = %s", datapath);
    gchar **split = g_strsplit((const gchar*) contents, "{DATAPATH}", -1);
    g_free(contents);
    contents = g_strjoinv(datapath, split);
    g_free(datapath);
    g_strfreev(split);
    g_free(path);
    gboolean b = mapserverapi_invoke(contents, "LAYERS=ocean&TRANSPARENT=true&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_xml&SRS=EPSG%3A4326&BBOX=-180.0,-90.0,180.0,90.0&WIDTH=500&HEIGHT=250", &body, &content_type, &body_length);
    g_free(contents);
    g_assert(b == TRUE);
    g_file_set_contents("./testresult.png", body, body_length, NULL);
    g_assert_cmpint(body_length, >, 1000);
    g_assert_cmpstr(content_type, ==, "image/png");
    g_free(content_type);
    mapserverapi_destroy();
}

int main(int argc, char** argv)
{
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/mapserverapi/new_free", test_mapserverapi_init);
    g_test_add_func("/mapserverapi/invoke", test_mapserverapi_invoke);
    gint res = g_test_run();
    return res;
}
