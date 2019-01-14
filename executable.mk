include config.mk

TARGET:=$(BUILDDIR)/mehustin
SOURCES=$(wildcard src/base/*.c)

LFLAGS=-rdynamic -L$(BASE)/lib/rocket/lib
LDLIBS+=-lm -ldl $(shell pkg-config --libs $(PKGS))

ifeq ($(DEBUG), 1)
LDLIBS+=-lrocket
else
LDLIBS+=-lrocket-player
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

