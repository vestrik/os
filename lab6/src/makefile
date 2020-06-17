CC=gcc -std=gnu99
CFLAGS=-I.

all: clsrv

clsrv : lib
	$(CC) client.c -L. -lmultmodulo -lpthread -o client
	$(CC) server.c -L. -lmultmodulo -lpthread -o server
	export LD_LIBRARY_PATH=/projects/vestrik-os/lab6/src/

lib : 
	$(CC) -c multmodulo.c -o multmodulo.o
	$(CC) -shared multmodulo.o -o libmultmodulo.so 

clean: 
	rm client server server.o client.o libmultmodulo.so multmodulo.o