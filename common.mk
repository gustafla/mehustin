OBJS=$(patsubst %.c,%.o,$(SOURCES:src/%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS)
ifeq ($(DEBUG),0)
	$(STRIP) $(TARGET)
endif

# compile target
$(BUILDDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<
