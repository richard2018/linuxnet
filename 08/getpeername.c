#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void)
{
	struct sockaddr_in addr;
	socklen_t len;
	int s = socket(AF_INET, SOCK_STREAM,0);
	
	getpeername(s, (struct sockaddr*)&addr, &len);
	printf("peer ")
	
	
	return 0;	
}