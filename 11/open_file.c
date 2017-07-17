#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
	int fd;
	ssize_t n;
	
	if(argc != 4){
		printf("socketpair error\n");
	if((fd = open(argv[2],atoi(argv[3])))<0)
		return(0);
		
	if((n = put_fd(atoi(argv[1]),"",1,fd))<0)
		return(0);
	
	return(0);	
	}	
}

