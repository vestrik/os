CC=gcc -std=gnu99
CFLAGS=-I.
all : tcpclient tcpserver udpclient udpserver dop
tcp : tcpclient tcpserver
udp : udpclient udpserver

dop : 
	$(CC) -o dopserv dopserv.c -lpthread $(CFLAGS)
	$(CC) -o dopclient dopclient.c -lpthread $(CFLAGS)

tcpclient : 
	$(CC) -o tcpclient tcpclient.c $(CFLAGS)

tcpserver : 
	$(CC) -o tcpserver tcpserver.c $(CFLAGS)
	
udpclient : 
	$(CC) -o udpclient udpclient.c $(CFLAGS)

udpserver : 
	$(CC) -o udpserver udpserver.c $(CFLAGS)

clean :
	rm -f tcpclient tcpserver udpclient udpserver dopserv dopclient server.txt client.txt core.*