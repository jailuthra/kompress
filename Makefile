CC=gcc
CFLAGS=

all: kompress

kompress: kompress.o queue.o bitstream.o
	$(CC) $(CFLAGS) kompress.o queue.o bitstream.o -o kompress

clean:
	rm -rf *.o
	rm kompress
