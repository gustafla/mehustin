# build and install settings
PREFIX?=release
BASE?=.
CC=gcc
PKGS=sdl2 gl
PKGS_MODULE=gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -DGL_GLEXT_PROTOTYPES

# debug and release settings
release:CFLAGS+=-O2 -ffast-math -s -DSYNC_PLAYER
debug:CFLAGS+=-Og -Wall -Wextra -pedantic -g -DDEBUG
