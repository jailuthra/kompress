CC=gcc
CFLAGS=

all: kompress dekompress

kompress: kompress.o queue.o bitstream.o
	$(CC) $(CFLAGS) kompress.o queue.o bitstream.o -o kompress

dekompress: dekompress.o
	$(CC) $(CFLAGS) dekompress.o -o dekompress

clean:
	rm -rf *.o
	rm kompress
	rm dekompress
