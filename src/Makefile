LIBS=-L. $(shell pkg-config --libs glib-2.0)
INCLUDES=-I. $(shell pkg-config --cflags glib-2.0)
CFLAGS+=-DG_LOG_DOMAIN=\"mapserverapi\" -fPIC -Wall

OBJECTS=random.o mapserverapi.o
BINARIES=libmapserverapi.so test_mapserverapi

all: $(OBJECTS) $(BINARIES)

clean:
	rm -f $(OBJECTS) $(BINARIES)

random.o: random.c random.h
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

mapserverapi.o: mapserverapi.c mapserverapi.h
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

test_mapserverapi: test_mapserverapi.c $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $@ $^

libmapserverapi.so: $(OBJECTS)
	$(CC) -shared $(CFLAGS) $(INCLUDES) $(LIBS) -o $@ $^

install: all
	cp -f libmapserverapi.so $(PREFIX)/lib/
	cp -f mapserverapi.h $(PREFIX)/include/
	cp -f test_mapserverapi $(PREFIX)/bin/
	chmod ug+rx $(PREFIX)/bin/test_mapserverapi

check: all
	./test_mapserverapi && echo "OK"

checks: all
	./test_mapserverapi INTEGRATION && echo "OK"

leak: all
	$(VALGRIND) ./test_mapserverapi

leaks: all
	$(VALGRIND) ./test_mapserverapi INTEGRATION

doc:
	echo RAS >/dev/null

coverage: all
	$(COVERAGE_INIT)
	./test_mapserverapi
	$(COVERAGE_CAPTURE)
	$(COVERAGE_REMOVE) "mapserverapi/src/test_mapserverapi.c"
	$(COVERAGE_EXPORT)

coverages: all
	$(COVERAGE_INIT)
	./test_mapserverapi INTEGRATION
	$(COVERAGE_CAPTURE)
	$(COVERAGE_REMOVE) "mapserverapi/src/test_mapserverapi.c"
	$(COVERAGE_EXPORT)

push::
	$(FTP_PUSH) mapserverapi
