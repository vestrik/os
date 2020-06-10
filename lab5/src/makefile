CC=gcc -std=c99
CFLAGS=-I.

all: mutex no_mutex

mutex: mutex.c
	$(CC) -o m mutex.c -lpthread $(CFLAGS)

no_mutex: no_mutex.c
	$(CC)  -o nom no_mutex.c -lpthread $(CFLAGS)

clean: 
	rm m nom