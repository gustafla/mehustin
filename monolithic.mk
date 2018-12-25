include config.mk

TARGET=demo
SOURCES=$(wildcard src/base/*.c) $(wildcard src/scene/*.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))

CFLAGS+=-DDEMO_MONOLITHIC
LDLIBS+=-lm -ldl $(shell pkg-config --libs $(PKGS))
release:LDLIBS+=-lrocket-player
debug:LDLIBS+=-lrocket

include common.mk
