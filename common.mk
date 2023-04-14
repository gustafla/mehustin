OBJS=$(patsubst %.c,%.o,$(SOURCES:%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS)
ifeq ($(DEBUG),0)
	$(STRIP) $(TARGET)
endif

# compile target
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<
