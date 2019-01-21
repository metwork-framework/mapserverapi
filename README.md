# mapserverapi

[//]: # (automatically generated from https://github.com/metwork-framework/resources/blob/master/cookiecutter/_%7B%7Bcookiecutter.repo%7D%7D/README.md)

## Status (master branch)
[![Drone CI](http://metwork-framework.org:8000/api/badges/metwork-framework/mapserverapi/status.svg)](http://metwork-framework.org:8000/metwork-framework/mapserverapi)
[![Maintenance](https://github.com/metwork-framework/resources/blob/master/badges/maintained.svg)]()

## What is it ?

This is a tiny C library to invoke [mapserver](http://mapserver.org) engine
as a library (with no daemon or CGI).

## Build

### Prerequisites

- mapserver installed
- standard compilation tools (gcc, make...)
- glib2 library and corresponding headers

### Building

To build the library you must specify:

- `MAPSERVER_LIB_DIR`: the full path of the directory containing `libmapserver.so`
- `PREFIX`: the full path of the directory where you want to install the library

For example:

```
make MAPSERVER_LIB_DIR=/opt/mapserver/lib PREFIX=/usr/local clean all install
```

### Testing

You must have `valgrind` tool installed.

```
make MAPSERVER_LIB_DIR=/opt/mapserver/lib PREFIX=/usr/local test
```

## Usage

### Compilation flags

The library use `pkg-config` tool. So you can use the following command to
get the compilation flags:

```
pkg-config --cflags --libs mapserverapi
```

If not found, you may have to add `PREFIX` at the end of `${PKG_CONFIG_PATH}`
environnement variable.

### In your code

```C
#include <mapserverapi.h>

// [...]

# Init the library (mandatory)
mapserverapi_init();

gchar *mapfile_content;
# [...]
# Put a mapfile content into mapfile_content string

# Set a WMS query_string (for this example)
gchar *query_string = "LAYERS=ocean&TRANSPARENT=true&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&EXCEPTIONS=application%2Fvnd.ogc.se_xml&SRS=EPSG%3A4326&BBOX=-180.0,-90.0,180.0,90.0&WIDTH=500&HEIGHT=250"

# Invoke mapserver
void *body;
gchar *content_type = NULL;
gsize body_length;
gboolean b = mapserverapi_invoke(mapfile_content, query_string, &body, &content_type, &body_length);
if (b == TRUE) {
    # you have the full body (PNG image in this example) in body variable (this buffer is managed by the library, don't free it by yourself !)
    # you have the body length in body_length variable.
    # you have the content_type of the body in content_type variable (you have to free it after use).

    # [...]

    # free content_type when you have finished with them
    # (but don't free body variable)
    g_free(content_type);
}

# Another way to use the library, invoke mapserver and get the body in a file
gchar *content_type2 = NULL;
gboolean

# Destroy the library
mapserverapi_destroy();
```


### Uninstalling

```
make MAPSERVER_LIB_DIR=/opt/mapserver/lib PREFIX=/usr/local uninstall
```




## Contributing guide

See [CONTRIBUTING.md](CONTRIBUTING.md) file.



## Code of Conduct

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) file.


