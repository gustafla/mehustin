include config.mk

TARGET=scene.so
SOURCES=scene.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

CFLAGS+=-fPIC
LFLAGS+=-shared

LDLIBS+=$(shell pkg-config --libs $(PKGS_MODULE))

include common.mk

.PHONY: install

install: $(TARGET)
	cp $(TARGET) $(PREFIX)/bin/
