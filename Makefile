CC=gcc

kompress: kompress.o queue.o
	$(CC) kompress.o queue.o -o kompress
