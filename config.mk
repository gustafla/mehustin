# build and install settings
PREFIX=release
CC=gcc
PKGS=sdl2 gl
PKGS_MODULE=gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -Ilib/stb -Ilib/rocket/lib -DGL_GLEXT_PROTOTYPES
LFLAGS+=-Llib/rocket/lib

# debug and release settings
release:CFLAGS+=-O2 -ffast-math -s -DSYNC_PLAYER
debug:CFLAGS+=-Og -Wall -g -DDEBUG
