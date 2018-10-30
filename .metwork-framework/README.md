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
make MAPSERVER_LIB_DIR=/opt/mapserver/lib PREFIX=/usr/local
```

## Usage

FIXME
