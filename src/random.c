#include <glib.h>
#include <stdio.h>
#include "random.h"

static GRand *__RAND = NULL;

GRand *__get_grand()
{
    if (__RAND == NULL) {
        __RAND = g_rand_new();
    }
    return __RAND;
}

gchar *get_unique_hexa_identifier()
{
    GRand *rand = __get_grand();
    gchar *res = g_malloc(sizeof(gchar) * 33);
    guint32 ri;
    int i;
    for (i = 0 ; i < 4 ; i++) {
        ri = g_rand_int(rand);
        sprintf(res + (i * 8) * sizeof(gchar),  "%08x", ri);
    }
    return res;
}
