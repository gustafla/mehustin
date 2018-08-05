include config.mk

TARGET=scene.so
SOURCES=scene.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

CFLAGS+=-fPIC
LFLAGS+=-shared

include common.mk

.PHONY: install

install: $(TARGET)
	cp $(TARGET) $(PREFIX)/bin/
