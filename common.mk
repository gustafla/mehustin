OBJS=$(patsubst %.c,%.o,$(SOURCES:%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) $(EXTRA_CFLAGS) $^ $(LDFLAGS) $(EXTRA_LDFLAGS) $(LDLIBS)
ifeq ($(DEBUG),0)
	$(STRIP) $(TARGET)
endif

# compile target
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<
