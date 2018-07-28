# common variables
TARGET=demo
CC=gcc
PREFIX=release
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -Ilib/stb -Ilib/rocket/lib -DGL_GLEXT_PROTOTYPES
LFLAGS+=-Llib/rocket/lib
LDLIBS+=$(shell pkg-config --libs $(PKGS)) -lm
SOURCES=main.c player.c demo.c gl_util.c read_file.c
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

# package release files
install: $(TARGET)
	install -d $(PREFIX)/data
	cp $(TARGET) $(PREFIX)/data/
	cp *.vert $(PREFIX)/data/
	cp *.frag $(PREFIX)/data/
	cp *.track $(PREFIX)/data/
	cp music.ogg $(PREFIX)/data/
	cp README.md $(PREFIX)/
	cp LICENSE $(PREFIX)/
	cp demo.sh $(PREFIX)/

clean:
	rm -f $(TARGET) $(OBJS)
