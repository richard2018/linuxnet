#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
	int fd[2],nbytes;
	pid_t childpid;
	char string[] = "Hello, World!\n";
	char readbuffer[80];
	
	pipe(fd);
	int w_fd = fd[1];
	int r_fd = fd[0];
	
	if((childpid = fork()) == -1)
	{
		perror("fork");
		exit(1);	
	}
	
	if(childpid == 0)
	{
		close(r_fd);
		write(w_fd,string,strlen(string));
		exit(0);		
	}
	else
	{
		close(w_fd);
		nbytes = read(r_fd, readbuffer,sizeof(readbuffer));
		printf("Received string:%s\n",readbuffer);		
	}
	
	return 0;

}

#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define K 1024
#define WRITELEN (128*K)
int main(void)
{
	int result = -1;
	int fd[2],nbytes;
	pid_t pid;
	char string[WRITELEN] = "你好，管道";
	char readbuffer[10*K];
	/* 文件描述符1用于写，文件描述符0用于读 */
	int *write_fd = &fd[1];
	int *read_fd = &fd[0];
	
	result = pipe(fd);
	if( -1 == result)
	{
		printf("建立管道失败\n");
		return -1;		
	}
	
	pid = fork();
	if( -1 == pid)
	{
		printf("fork 进程失败\n");
		return -1;
	}
	
	if( 0 == pid)
	{
		/* 子进程 */
		int write_size = WRITELEN;
		result = 0;
		close(*read_fd);
		while( write_size >= 0)
		{
			result = write(*write_fd,string,write_size);
			if(result >0)
			{
				write_size -= result;
				printf("写入%d个数据，剩余%d个数据\n",result,write_size);
			}
			else
			{
				sleep(10);		
			}
		}
		return 0;	
	}
	else
	{
		/* 父进程 */
		close(*write_fd);
		while(1)
		{
			nbytes = read(*read_fd, readbuffer,sizeof(readbuffer));
			if(nbytes <= 0)
			{
				printf("没有数据写入了\n");
				break;		
			}	
			printf("接收到%d个数据，内容为:”%s“\n",nbytes,readbuffer);
			//sleep(100);
		}
				
	}
	
	return 0;
}

#endif 