DEBUG?=1

# debug and release settings
ifeq ($(DEBUG),0)
BUILDDIR:=release
CFLAGS+=-Os -ftree-vectorize -ffast-math -flto -DSYNC_PLAYER
else
BUILDDIR:=debug
CFLAGS+=-Og -g -DDEBUG
endif

# build and install settings
PREFIX?=demo
BASE?=.
CC:=gcc
STRIP:=strip --strip-all
CFLAGS+=-std=c99 -Wall -Wextra -Wpedantic -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -I$(BUILDDIR)
LDFLAGS+=-L$(BASE)/lib/rocket/lib -L$(BASE)/$(BUILDDIR)

PKGS_EXECUTABLE:=sdl2
ifeq ($(GLES),1)
PKGS_MODULE:=glesv2
CFLAGS+=-DGL_MAJOR=2 -DGL_MINOR=0 -DGL_PROFILE=ES -DGLES
else
PKGS_MODULE:=gl
CFLAGS+=-DGL_MAJOR=3 -DGL_MINOR=3 -DGL_PROFILE=CORE
endif

MODULE_PATH=$(BUILDDIR)/libdemo.so
SHADERS=$(wildcard shaders/*)
