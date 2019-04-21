include config.mk

TARGET=$(BUILDDIR)/demo
SOURCES=$(wildcard src/base/*.c) $(wildcard src/scene/*.c)

CFLAGS+=$(shell pkg-config --cflags $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -DDEMO_MONOLITHIC
LDLIBS+=$(shell pkg-config --libs $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -lm -ldl

ifeq ($(DEBUG),0)
LDLIBS+=-lrocket-player
else
LDLIBS+=-lrocket
endif

include common.mk
