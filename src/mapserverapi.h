#ifndef MAPSERVERAPI_H_
#define MAPSERVERAPI_H_

#include <glib.h>

void mapserverapi_init();
void mapserverapi_destroy();
gboolean mapserverapi_invoke(const gchar *mapfile_content, const gchar *query_string, void **body, gchar **content_type, gsize *body_length);

#endif /* MAPSERVERAPI_H_ */
