OBJS=$(patsubst %.c,%.o,$(SOURCES))

# link target
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LFLAGS) $(LDLIBS)

# compile target
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release

debug: $(TARGET)

release: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
