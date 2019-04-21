include config.mk

TARGET:=$(BUILDDIR)/mehustin
SOURCES=$(wildcard src/base/*.c)

CFLAGS+=$(shell pkg-config --cflags $(PKGS_EXECUTABLE))
LDFLAGS+=-rdynamic -L$(BASE)/lib/rocket/lib
LDLIBS+=$(shell pkg-config --libs $(PKGS_EXECUTABLE)) -lm -ldl

ifeq ($(DEBUG),0)
LDLIBS+=-lrocket-player
else
LDLIBS+=-lrocket
endif

include common.mk

.PHONY: install

install: $(TARGET) LICENSE demo.sh
	-mkdir -p $(PREFIX)/bin
	-mkdir -p $(PREFIX)/src
	cp $(TARGET) $(PREFIX)/bin/
	cp LICENSE $(PREFIX)/
	cp demo.sh $(PREFIX)/
	-git clone ./ $(PREFIX)/src
	-cd $(PREFIX)/src; git remote remove origin

