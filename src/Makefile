ifndef MAPSERVER_LIB_DIR
$(error MAPSERVER_LIB_DIR is not set, see README.md file)
endif
ifndef PREFIX
$(error PREFIX is not set, see README.md file)
endif

_LDFLAGS=$(LDFLAGS) -L. $(shell pkg-config --libs glib-2.0) -L$(MAPSERVER_LIB_DIR) -L$(PREFIX)/lib -lmapserver
_CFLAGS=$(CFLAGS) -I. $(shell pkg-config --cflags glib-2.0) -I$(PREFIX)/include -fPIC -Wall
CC=gcc

OBJECTS=random.o mapserverapi.o
BINARIES=libmapserverapi.so test_mapserverapi

all: $(OBJECTS) $(BINARIES) mapserverapi.pc

clean:
	rm -f $(OBJECTS) $(BINARIES) core.* vgcore.* testresult.png mapserverapi.pc

mapserverapi.pc: mapserverapi.pc.pc
	cat $< |sed 's~@@@PREFIX@@@~$(PREFIX)~g' |sed 's~@@@MAPSERVER_LIB_DIR@@@~$(MAPSERVER_LIB_DIR)~g' >$@

random.o: random.c random.h
	$(CC) -c $(_CFLAGS) -o $@ $<

mapserverapi.o: mapserverapi.c mapserverapi.h
	$(CC) -c $(_CFLAGS) -o $@ $<

test_mapserverapi: test_mapserverapi.c $(OBJECTS)
	$(CC) $(_CFLAGS) $(_LDFLAGS) -o $@ $^

libmapserverapi.so: $(OBJECTS)
	$(CC) -shared $(_CFLAGS) $(_LDFLAGS) -o $@ $^

install: all
	if ! test -d $(PREFIX)/lib; then mkdir -p $(PREFIX)/lib; fi
	if ! test -d $(PREFIX)/include; then mkdir -p $(PREFIX)/include; fi
	cp -f libmapserverapi.so $(PREFIX)/lib/
	cp -f mapserverapi.h $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/libmapserverapi.so
	rm -f $(PREFIX)/include/mapserverapi.h

test: all
	valgrind --show-possibly-lost=no --leak-check=full ./test_mapserverapi
