OBJS=$(patsubst %.c,%.o,$(SOURCES:src/%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	$(info Linking $@)
	@mkdir -p $(@D)
	@$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS)
ifeq ($(DEBUG),0)
	$(info Stripping $@)
	@$(STRIP) $(TARGET)
endif

# compile target
$(BUILDDIR)/%.o: src/%.c
	$(info Compiling $@)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
