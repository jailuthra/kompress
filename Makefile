CC=gcc
CFLAGS=

all: kompress dekompress

kompress: kompress.o queue.o bitstream.o kmp.h
	$(CC) $(CFLAGS) kompress.o queue.o bitstream.o -o kompress

dekompress: dekompress.o bitstream.o kmp.h
	$(CC) $(CFLAGS) dekompress.o bitstream.o -o dekompress

clean:
	rm -rf *.o
	rm -f kompress
	rm -f dekompress
