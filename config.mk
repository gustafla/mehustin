DEBUG?=1

# build and install settings
PREFIX=demo
BASE=.
CC=gcc
STRIP=strip --strip-all
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic
EXTRA_CFLAGS=-I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -I$(BUILDDIR)
EXTRA_LDFLAGS=-L$(BASE)/lib/rocket/lib -L$(BASE)/$(BUILDDIR)

# debug and release settings
ifeq ($(DEBUG),0)
BUILDDIR=release
CFLAGS+=-Os -ftree-vectorize -ffast-math -flto
EXTRA_CFLAGS+=-DSYNC_PLAYER
else
BUILDDIR=debug
CFLAGS+=-Og -g
EXTRA_CFLAGS+=-DDEBUG
endif

PKGS_EXECUTABLE=sdl2
ifeq ($(GLES),1)
PKGS_MODULE=glesv2
EXTRA_CFLAGS+=-DGL_MAJOR=2 -DGL_MINOR=0 -DGL_PROFILE=ES -DGLES
else
ifeq ($(MINGW),1)
LDLIBS+=-lglew32 -lopengl32
else
PKGS_MODULE=gl
endif
EXTRA_CFLAGS+=-DGL_MAJOR=3 -DGL_MINOR=3 -DGL_PROFILE=CORE
endif

MODULE_PATH=$(BUILDDIR)/libdemo.so
SHADERS=$(wildcard shaders/*)
