include config.mk

TARGET=$(MODULE_PATH)
SOURCES=$(wildcard src/scene/*.c)

EXTRA_CFLAGS+=$(shell pkg-config --cflags $(PKGS_MODULE)) -fPIC
EXTRA_LDFLAGS+=-shared
LDLIBS+=$(shell pkg-config --libs $(PKGS_MODULE))

.PHONY: module

module: $(BUILDDIR)/shader_code.h $(TARGET)

$(BUILDDIR)/shader_code.h: $(SHADERS)
	@mkdir -p $(@D)
	@rm -f $@
	for u in $$(cat $^ | tr -s ' ' | grep '^uniform' | tr -d ';' | cut -d ' ' -f 3 | sort | uniq);\
	do echo "#define VAR_$$u \"$$u\"" >> $@; done

include common.mk

.PHONY: install

install: $(TARGET)
	-mkdir -p $(PREFIX)/bin
	cp $(TARGET) $(PREFIX)/bin/
	-cp -r shaders $(PREFIX)/bin/
	-cp *.track $(PREFIX)/bin/
	-cp music.ogg $(PREFIX)/bin/
	-cp *.ttf $(PREFIX)/bin/
