DEBUG?=1

# debug and release settings
ifeq ($(DEBUG),0)
BUILDDIR:=release
CFLAGS+=-Os -ftree-vectorize -ffast-math -DSYNC_PLAYER
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
PKGS_MODULE:=glesv2

MODULE_PATH=$(BUILDDIR)/libdemo.so

SHADERS=$(wildcard *.frag) $(wildcard *.vert)
