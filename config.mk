DEBUG?=1

# debug and release settings
ifeq ($(DEBUG), 1)
BUILDDIR:=debug
CFLAGS:=-Og -g -DDEBUG -fsanitize=address -fsanitize=undefined
else
BUILDDIR:=release
CFLAGS:=-O2 -ffast-math -s -DSYNC_PLAYER
endif

# build and install settings
PREFIX?=release
BASE?=.
CC=gcc
PKGS=sdl2 glesv2
PKGS_MODULE=glesv2
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -Wall -Wextra -Wpedantic -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -I$(BASE)/lib/cglm/include -DM_PI=3.14159265f -DM_PI_2="(2*M_PI)" -DM_PI_4="(4*M_PI)"
