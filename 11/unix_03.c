001		#include <sys/types.h>
002		#include <sys/socket.h>
003		#include <linux/un.h>
004		#include <string.h>
005		#include <signal.h>
006		#include <stdio.h>
007		#include <errno.h>
008		#include <unistd.h>
009		
010		/*
011		*	从fd中接收消息，并将文件描述符放在指针recvfd中
012		*/
013		ssize_t recv_fd(int fd, void *data, size_t bytes, int *recvfd)
014		{
015			struct msghdr msghdr_recv;/*接收消息接收*/
016			struct iovec iov[1];/*接收数据的向量*/
017			size_t n;
018			int newfd;
019			
020			union{
021				struct cmsghdr cm;
022				char control[CMSG_SPACE(sizeof(int))];	
023			}control_un;
024			struct cmsghdr *pcmsghdr;/*消息头部*/
025			msghdr_recv.msg_control = control_un.control;/*控制消息*/
026			msghdr_recv.msg_controllen = sizeof(control_un.control);/*控制消息的长度*/
027			
028			msghdr_recv.msg_name = NULL;/*消息的名称为空*/
029			msghdr_recv.msg_namelen = 0;/*消息的长度为空*/
030			
031			iov[0].iov_base = data;/*向量的数据为传入的数据*/
032			iov[0].iov_len = bytes;/*向量的长度为传入数据的长度*/
033			msghdr_recv.msg_iov = iov;/*消息向量指针*/
034			msghdr_recv.msg_iovlen = 1;/*消息向量的个数为1个*/
035			if((n = recvmsg(fd, &msghdr_recv, 0))<=0)/*接收消息*/
036				return n;
037				
038			if((pcmsghdr = CMSG_FIRSTHDR(&msghdr_recv))!= NULL &&/*获得消息的头部*/
039				pcmsghdr->cmsg_len == CMSG_LEN(sizeof(int))){/*获得小弟的长度为int*/
040				if(pcmsghdr->cmsg_level != SOL_SOCKET)/*消息的level应该为SOL_SOCKET*/
041					printf("control level != SOL_SOCKET\n");
042				
043				if(pcmsghdr->cmsg_type != SCM_RIGHTS)/*消息的类型判断*/
044					printf("control type != SCM_RIGHTS\n");
045					
046					*recvfd = *((int*)CMSG_DATA(pcmsghdr));		/*获得打开文件的描述符*/
047			}else
048				*recvfd = -1;
049				
050			return n;/*返回接收消息的长度*/
051		}
052		
053		int my_open(const char *pathname, int mode)
054		{
055			int fd, sockfd[2],status;
056			pid_t childpid;
057			char c, argsockfd[10],argmode[10];
058			
059			socketpair(AF_LOCAL,SOCK_STREAM,0,sockfd);/*建立socket*/
060			if((childpid = fork())==0){/*子进程*/
061				close(sockfd[0]);/*关闭sockfd[0]*/
062				snprintf(argsockfd, sizeof(argsockfd),"%d",sockfd[1]);/*socket描述符*/
063				snprintf(argmode, sizeof(argmode),"%d",mode);/*打开文件的方式*/
064				execl("./openfile","openfile",argsockfd, pathname,argmode,(char*)NULL)	;/*执行进程A*/
065				printf("execl error\n");
066			}	
067			/*父进程*/
068			close(sockfd[1]);
069			/*等待子进程结束*/
070			waitpid(childpid, &status,0);
071			
072			if(WIFEXITED(status)==0){/*判断子进程是否结束*/
073				printf("child did not terminate\n")	;
074			if((status = WEXITSTATUS(status))==0){/*子进程结束*/
075				recv_fd(sockfd[0],&c,1,&fd);/*接收进程A打开的文件描述符*/
076			}	else{
077				errno = status;
078				fd = -1;	
079			}	
080			
081			close(sockfd[0]);/*关闭sockfd[0]*/
082			return fd;/*返回进程A打开文件的描述符*/
083		
084			}
085		}
086		
087		#define BUFFSIZE 256	/*接收的缓冲区大小*/
088		int main(int argc, char *argv[])
089		{
090			int fd, n;
091			char buff[BUFFSIZE];	/*接收缓冲区*/
092			
093			if(argc !=2)
094				printf("error argc\n");
095				
096			if((fd = my_open(argv[1], O_RDONLY))<0)/*获得进程A打开的文件描述符*/
097				printf("can't open %s\n",argv[1]);
098				
099			while((n = read(fd, buff, BUFFSIZE))>0)/*读取数据*/
100			write(1,buff,n);/*写入标准输出*/
101			
102			return(0);	
103		}
