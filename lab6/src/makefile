CC=gcc -std=gnu99
CFLAGS=-I.

all: client server

client :
	$(CC) -o client client.c -lpthread $(CFLAGS)

server :
	$(CC) -o server server.c -lpthread $(CFLAGS)

clean: 
	rm client server