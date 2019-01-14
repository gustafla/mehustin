OBJS=$(patsubst %.c,%.o,$(SOURCES:src/%=$(BUILDDIR)/%))

# link target
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LFLAGS) $(LDLIBS)

# compile target
$(BUILDDIR)/%.o: $(SOURCES)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release

debug: $(TARGET)

release: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
