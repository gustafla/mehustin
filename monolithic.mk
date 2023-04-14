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

monolith: $(BUILDDIR)/shader_code.h $(BUILDDIR)/music.h $(BUILDDIR)/sync_tracks.h $(TARGET)
	echo done

$(BUILDDIR)/music.h: music.ogg
	@mkdir -p $(@D)
	xxd -i $^ $@

$(BUILDDIR)/shader_code.h: $(SHADERS)
	@mkdir -p $(@D)
	shader_minifier.exe -o $@ $^

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
