CC=gcc -std=gnu99
CFLAGS=-I.

all: f d mutex no_mutex

mutex: 
	$(CC) -o m mutex.c -lpthread $(CFLAGS)

no_mutex: 
	$(CC)  -o nom no_mutex.c -lpthread $(CFLAGS)

f :
	$(CC) -o f factorial.c -lpthread $(CFLAGS)

d :
	$(CC) -o d deadlock.c -lpthread $(CFLAGS)

clean: 
	rm m nom f d