sort.o:
	gcc -c sort.c
sort: sort.o
	gcc -o sort sort.o -l pthread
all: sort
clean:
	rm sort.o sort

.DEFAULT_GOAL := all
