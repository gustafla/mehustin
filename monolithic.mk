include config.mk

TARGET=$(BUILDDIR)/demo
SOURCES=$(wildcard src/base/*.c) $(wildcard src/scene/*.c) lib/stb/stb_vorbis.c

CFLAGS+=$(shell pkg-config --cflags $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -DMONOLITH
LDLIBS+=$(shell pkg-config --libs $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -lm

ifeq ($(DEBUG),0)
LDLIBS+=-lrocket-player
STRIP=sstrip
else
LDLIBS+=-lrocket
endif

.PHONY: monolith

monolith: $(BUILDDIR)/shader_code.h $(BUILDDIR)/music.h $(TARGET)
	echo done

$(BUILDDIR)/music.h: music.ogg
	@mkdir -p $(@D)
	xxd -i music.ogg $(BUILDDIR)/music.h

$(BUILDDIR)/shader_code.h: $(SHADERS)
	@mkdir -p $(@D)
	shader_minifier.exe -o $(BUILDDIR)/shader_code.h $(SHADERS)

include common.mk
