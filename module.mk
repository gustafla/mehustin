include config.mk

TARGET=$(MODULE_PATH)
SOURCES=$(wildcard src/scene/*.c)

EXTRA_CFLAGS+=$(shell pkg-config --cflags $(PKGS_MODULE)) -fPIC
EXTRA_LDFLAGS+=-shared
LDLIBS+=$(shell pkg-config --libs $(PKGS_MODULE))

include common.mk

.PHONY: install

install: $(TARGET)
	-mkdir -p $(PREFIX)/bin
	cp $(TARGET) $(PREFIX)/bin/
	-cp -r shaders $(PREFIX)/bin/
	-cp *.track $(PREFIX)/bin/
	-cp music.ogg $(PREFIX)/bin/
