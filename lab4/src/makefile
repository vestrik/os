CC=gcc
CFLAGS=-I.

all : ./1/par

par : ./1/utils.o ./1/find_min_max.o ./1/utils.h ./1/find_min_max.h
	$(CC) -o ./1/par ./1/utils.o ./1/find_min_max.o ./1/parallel_min_max.c $(CFLAGS)

utils.o : ./1/utils.h
	$(CC) -o ./1/utils.o -c ./1/utils.c $(CFLAGS)

find_min_max.o : ./1/utils.h ./1/find_min_max.h
	$(CC) -o ./1/find_min_max.o -c ./1/find_min_max.c $(CFLAGS)

clean :
	rm ./1/utils.o ./1/find_min_max.o ./1/par