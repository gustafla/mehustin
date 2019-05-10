DEBUG?=1

# debug and release settings
ifeq ($(DEBUG),0)
BUILDDIR:=release
CFLAGS+=-O2 -ftree-vectorize -ffast-math -DSYNC_PLAYER
else
BUILDDIR:=debug
CFLAGS+=-Og -g -DDEBUG
endif

# build and install settings
PREFIX?=demo
BASE?=.
CC:=gcc
STRIP:=strip --strip-all
CFLAGS+=-std=c99 -Wall -Wextra -Wpedantic -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib
LDFLAGS+=-L$(BASE)/lib/rocket/lib -L$(BASE)/$(BUILDDIR)

PKGS_EXECUTABLE:=sdl2
PKGS_MODULE:=glesv2

