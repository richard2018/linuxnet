CC = gcc
all:client server
	
client:tcp_process.o tcp_client.o
	$(CC) -o client tcp_process.o tcp_client.o -g
server:tcp_process.o tcp_server.o
	$(CC) -o server tcp_process.o tcp_server.o -g
tcp_process.o:
	$(CC) -c -o tcp_process.o tcp_process.c -g
clean:
	rm -f client server *.o
