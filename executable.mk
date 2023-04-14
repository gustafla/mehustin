include config.mk

ifeq ($(MINGW),1)
TARGET=$(BUILDDIR)/mehustin.exe
RUNSCR=src/run.bat
else
TARGET=$(BUILDDIR)/mehustin
RUNSCR=src/run.sh
endif

SOURCES=$(wildcard src/base/*.c) lib/stb/stb_vorbis.c

EXTRA_CFLAGS+=$(shell pkg-config --cflags $(PKGS_EXECUTABLE))
LDLIBS+=$(shell pkg-config --libs $(PKGS_EXECUTABLE)) -lm

ifeq ($(DEBUG),0)
LDLIBS+=-lrocket-player -ldemo
STRIP=sstrip
else
LDLIBS+=-lrocket -ldl
EXTRA_CFLAGS+=-DDEMO_RTDL -DMODULE_PATH=\"$(MODULE_PATH)\"
endif

include common.mk

.PHONY: install

install: $(TARGET) LICENSE $(RUNSCR)
	cp $(TARGET) $(PREFIX)/bin/
	cp LICENSE $(PREFIX)/
	cp $(RUNSCR) $(PREFIX)/
	-git clone ./ $(PREFIX)/src
	-cd $(PREFIX)/src; rm -rf .git

