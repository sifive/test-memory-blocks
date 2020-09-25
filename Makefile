
SRC = test-memory-blocks.c
HDR = test-memory-blocks.h

test-memory-blocks: $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f *.i *.s *.o test-memory-blocks test-memory-blocks.hex

