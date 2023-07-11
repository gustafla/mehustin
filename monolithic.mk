include config.mk

ifeq ($(MINGW),1)
TARGET=$(BUILDDIR)/demo.exe
else
TARGET=$(BUILDDIR)/demo
endif
SOURCES=$(wildcard src/base/*.c) $(wildcard src/scene/*.c) lib/stb/stb_vorbis.c $(BUILDDIR)/shader_code.c

EXTRA_CFLAGS+=$(shell pkg-config --cflags $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -DMONOLITH
LDLIBS+=$(shell pkg-config --libs $(PKGS_EXECUTABLE) $(PKGS_MODULE)) -lm

ifeq ($(DEBUG),0)
LDLIBS+=-lrocket-player
STRIP=sstrip
else
LDLIBS+=-lrocket
endif

.PHONY: monolith

monolith: $(BUILDDIR)/shader_code.c $(BUILDDIR)/shader_code.h $(BUILDDIR)/music.h $(BUILDDIR)/sync_tracks.h $(BUILDDIR)/font.h $(TARGET)
	echo done

$(BUILDDIR)/music.h: music.ogg
	@mkdir -p $(@D)
	xxd -i $^ $@

$(BUILDDIR)/font.h: OpenSans-Bold.ttf
	@mkdir -p $(@D)
	xxd -i $^ $@

$(BUILDDIR)/shader_code.c: $(SHADERS)
	@mkdir -p $(@D)
	shader_minifier.exe -o $@ $^

$(BUILDDIR)/shader_code.h: $(BUILDDIR)/shader_code.c
	grep '^\(const char \*.* =\|#\)' $^ | sed 's/const/extern const/;s/ =/;/' > $@

$(BUILDDIR)/sync_tracks.h: $(wildcard *.track)
	@mkdir -p $(@D)
	@rm -f $@
	for tf in $^; do xxd -i $$tf >> $@; done
	printf 'const char *sync_track_filenames =' >> $@
	for tf in $^; do printf \ \"$$tf\\\\0\" >> $@; done
	printf ';\n' >> $@
	printf 'const unsigned char *sync_track_data[] = {\n' >> $@
	for tf in $^; do echo "$${tf}," | sed 's/[-\.]/_/g' >> $@; done
	printf '};\n' >> $@
	printf 'const unsigned int sync_track_lens[] = {\n' >> $@
	for tf in $^; do stat -c %s $$tf >> $@; printf ',' >> $@; done
	printf '};\n' >> $@


include common.mk

.PHONY: install

install: $(TARGET) LICENSE
	-mkdir -p $(PREFIX)
	cp $(TARGET) $(PREFIX)
	cp LICENSE $(PREFIX)
	-git clone ./ $(PREFIX)/src
	-cd $(PREFIX)/src; rm -rf .git

