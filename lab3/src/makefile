CC=gcc -std=c99
CFLAGS=-I.

all : par seq exec

exec : exec.c utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o exec find_min_max.o utils.o exec.c $(CFLAGS)

seq : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o seq find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

par : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o par utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o seq par
