CC=gcc

all: kompress

kompress: kompress.o queue.o
	$(CC) kompress.o queue.o -o kompress

clean:
	rm -rf *.o
	rm kompress
