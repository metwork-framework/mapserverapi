#ifndef MAPSERVERAPI_MAPSERVER_H_
#define MAPSERVERAPI_MAPSERVER_H_

#ifndef USE_GEOS
#define USE_GEOS 1
#endif

int msCGIHandler(const char *query_string, void **out_buffer, size_t *buffer_length);
void msIO_resetHandlers(void);
int msSetup(void);
void msCleanup(void);

#endif /* MAPSERVERAPI_MAPSERVER_H_ */
