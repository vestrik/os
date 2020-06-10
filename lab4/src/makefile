CC=gcc -std=gnu99
CFLAGS=-I.

all : par zombie psum memo

par : ./1/utils.o ./1/find_min_max.o ./1/utils.h ./1/find_min_max.h
	$(CC) -o par ./1/utils.o ./1/find_min_max.o ./1/parallel_min_max.c $(CFLAGS)

utils.o : /projects/vestrik-os/lab3/src/utils.h
	$(CC) -o utils.o -c /projects/vestrik-os/lab3/src/utils.c $(CFLAGS)

zombie : zombie.c
	$(CC) -o zombie zombie.c $(CFLAGS)

memo : process_memory.c
	$(CC) -o memo process_memory.c $(CFLAGS)

count_sum.o : /projects/vestrik-os/lab3/src/utils.h count_sum.h
	$(CC) -o count_sum.o -c count_sum.c $(CFLAGS)

psum : utils.o count_sum.o /projects/vestrik-os/lab3/src/utils.h count_sum.h
	$(CC) -o psum utils.o count_sum.o parallel_sum.c -lpthread  $(CFLAGS)

clean :
	rm ./1/file.txt ./1/utils.o ./1/find_min_max.o par zombie psum utils.o count_sum.o memo