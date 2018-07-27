TARGET=demo
CC=gcc
PREFIX=~/.local
PKGS=sdl2
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -Os -Ilib/stb
LDLIBS+=$(shell pkg-config --libs $(PKGS)) -lm

debug:CFLAGS+=-g -DDEBUG

SOURCES=main.c player.c demo.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(TARGET): $(OBJS)
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean install debug

debug: $(TARGET)

install: $(TARGET)
	install -d $(PREFIX)/bin/
	cp $(TARGET) $(PREFIX)/bin/

clean:
	rm -f $(TARGET) $(OBJS)
