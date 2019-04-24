DEBUG?=1

# debug and release settings
ifeq ($(DEBUG),0)
BUILDDIR:=release
CFLAGS+=-O2 -ffast-math -DSYNC_PLAYER
else
BUILDDIR:=debug
CFLAGS+=-Og -g -DDEBUG -fsanitize=address -fsanitize=undefined
endif

# build and install settings
PREFIX?=demo
BASE?=.
CC:=gcc
STRIP:=strip --strip-all
CFLAGS+=-std=c99 -Wall -Wextra -Wpedantic -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -I$(BASE)/lib/cglm/include -DM_PI=3.14159265f -DM_PI_2="(2*M_PI)" -DM_PI_4="(4*M_PI)"

PKGS_EXECUTABLE:=sdl2
PKGS_MODULE:=glesv2

