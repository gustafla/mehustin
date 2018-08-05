# build and install settings
PREFIX=release
CC=gcc
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -Ilib/stb -Ilib/rocket/lib -DGL_GLEXT_PROTOTYPES
LFLAGS+=-Llib/rocket/lib
LDLIBS+=$(shell pkg-config --libs $(PKGS)) -lm

# debug and release settings
release:LDLIBS+=-lrocket-player
release:CFLAGS+=-O2 -ffast-math -s -DSYNC_PLAYER
debug:LDLIBS+=-lrocket
debug:CFLAGS+=-Og -Wall -g -DDEBUG
