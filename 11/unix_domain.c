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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFSIZE 256
int main(int argc, char *argv[])
{
	int fd, n;
	char buff[BUFFSIZE];
	
	if(argc !=2)
		printf("error argc\n");
		
	if((fd = my_open(argv[1], O_RDONLY))<0)
		printf("can't open %s\n",argv[1]);
		
	while((n = read(fd, buff, BUFFSIZE))>0)
	write(1,buff,n);
	
	return(0);	
}

int my_open(const char *pathname, int mode)
{
	int fd, sockfd[2],status;
	pid_t childpid;
	char c, argsockfd[10],argmode[10];
	
	socketpair(AF_LOCAL,SOCK_STREAM,0,sockfd);
	if((childpid = fork())==0){
		close(sockfd[0]);
		snprintf(argsockfd, sizeof(argsockfd),"%d",sockfd[1]);
		snprintf(argmode, sizeof(argmode),"%d",mode);
		execl("./openfile","openfile",argsockfd, pathname,argmode,(char*)NULL)	;
		printf("execl error\n");
	}	
	
	close(sockfd[1]);
	
	waitpid(childpid, &status,0);
	
	if(WIFEXITED(status)==0){
		printf("child did not terminate\n")	;
	if((status = WEXITSTATUS(status))==0){
		get_fd(sockfd[0],&c,1,&fd);
	}	else{
		errno = status;
		fd = -1;	
	}	
	
	close(sockfd[0]);
	return fd;

	}
}
