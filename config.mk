# build and install settings
PREFIX?=release
BASE?=.
CC=gcc
PKGS=sdl2 glesv2
PKGS_MODULE=glesv2
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) -std=c99 -I$(BASE) -I$(BASE)/lib/stb -I$(BASE)/lib/rocket/lib -I$(BASE)/lib/cglm/include -DM_PI=3.14159265f -DM_PI_2="(2*M_PI)" -DM_PI_4="(4*M_PI)"

# debug and release settings
release:CFLAGS+=-O2 -ffast-math -s -DSYNC_PLAYER
debug:CFLAGS+=-Og -Wall -Wextra -pedantic -g -DDEBUG
