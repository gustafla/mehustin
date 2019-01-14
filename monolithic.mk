include config.mk

TARGET=$(BUILDDIR)/demo
SOURCES=$(wildcard src/base/*.c) $(wildcard src/scene/*.c)

CFLAGS+=-DDEMO_MONOLITHIC
LDLIBS+=-lm -ldl $(shell pkg-config --libs $(PKGS))

ifeq ($(DEBUG), 0)
LDLIBS+=-lrocket-player
else
LDLIBS+=-lrocket
endif

include common.mk
