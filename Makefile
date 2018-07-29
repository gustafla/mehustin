# common variables
TARGET=demo
CC=gcc
PREFIX=release
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -Ilib/stb -Ilib/rocket/lib -DGL_GLEXT_PROTOTYPES
LFLAGS+=-Llib/rocket/lib
LDLIBS+=$(shell pkg-config --libs $(PKGS)) -lm
SOURCES=main.c player.c demo.c gl_util.c read_file.c gl_matrix.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

# debug and release variables
release:LDLIBS+=-lrocket-player
release:CFLAGS+=-O2 -ffast-math -s -DSYNC_PLAYER
debug:LDLIBS+=-lrocket
debug:CFLAGS+=-Wall -g -DDEBUG

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
install: $(TARGET) LICENSE demo.sh
	install -d $(PREFIX)/bin
	install -d $(PREFIX)/src
	cp $(TARGET) $(PREFIX)/bin/
	cp *.vert $(PREFIX)/bin/ || :
	cp *.frag $(PREFIX)/bin/ || :
	cp *.track $(PREFIX)/bin/ || :
	cp music.ogg $(PREFIX)/bin/ || :
	cp README.md $(PREFIX)/ || :
	cp LICENSE $(PREFIX)/
	cp demo.sh $(PREFIX)/
	git clone ./ $(PREFIX)/src
	cd $(PREFIX)/src; git remote remove origin

clean:
	rm -f $(TARGET) $(OBJS)
