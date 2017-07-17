
CC = gcc
all:client server
	
client:tcp_process.o tcp_client.o
	$(CC) -o client tcp_process.o tcp_client.o
server:tcp_process.o tcp_server.o
	$(CC) -o server tcp_process.o tcp_server.o
clean:
	rm -f client server *.o