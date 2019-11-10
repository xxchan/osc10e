CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: buffer.o
	$(CC) $(CFLAGS) -o example buffer.o $(PTHREADS)

buffer.o: buffer.h buffer.c
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

clean:
	rm -f example
	rm -f *.o
