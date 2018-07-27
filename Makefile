# common variables
TARGET=demo
CC=gcc
PREFIX=~/.local
PKGS=sdl2
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -Ilib/stb -Ilib/rocket/lib
LFLAGS+=-Llib/rocket/lib
LDLIBS+=$(shell pkg-config --libs $(PKGS)) -lm
SOURCES=main.c player.c demo.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

# debug and release variables
release:LDLIBS+=-lrocket-player
release:CFLAGS+=-Os -s -DSYNC_PLAYER
debug:LDLIBS+=-lrocket
debug:CFLAGS+=-g -DDEBUG

# link target
$(TARGET): $(OBJS)
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LFLAGS) $(LDLIBS)

# compile target
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean install debug release

debug: $(TARGET)

release: $(TARGET)

install: $(TARGET)
	install -d $(PREFIX)/bin/
	cp $(TARGET) $(PREFIX)/bin/

clean:
	rm -f $(TARGET) $(OBJS)
