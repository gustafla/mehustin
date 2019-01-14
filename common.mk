OBJS=$(patsubst %.c,%.o,$(SOURCES:src/%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LFLAGS) $(LDLIBS)

# compile target
$(BUILDDIR)/%.o: $(SOURCES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
