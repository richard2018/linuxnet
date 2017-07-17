01		#include <sys/types.h>
02		#include <sys/socket.h>
03		#include <linux/un.h>
04		#include <string.h>
05		#include <signal.h>
06		#include <stdio.h>
07		#include <errno.h>
08		#include <unistd.h>
09		
10		ssize_t send_fd(int fd, void *data, size_t bytes, int sendfd)
11		{
12			struct msghdr msghdr_send;	/*发送消息*/
13			struct iovec iov[1];		/*向量*/
14			size_t n;					/*大小*/
15			int newfd;					/*文件描述符*/
16			/* 方面操作msg的结构 */
17			union{
18				struct cmsghdr cm;		/*control msg结构*/
19				char control[CMSG_SPACE(sizeof(int))];	/*字符指针，方便控制*/
20			}control_un;
21			struct cmsghdr *pcmsghdr=NULL;/*控制头部的指针*/
22			msghdr_send.msg_control = control_un.control;/*控制消息*/
23			msghdr_send.msg_controllen = sizeof(control_un.control);/*长度*/
24			
25			pcmsghdr = CMSG_FIRSTHDR(&msghdr_send);/*取得第一个消息头*/
26			pcmsghdr->cmsg_len = CMSG_LEN(sizeof(int));/*获得长度*/
27			pcmsghdr->cmsg_level = SOL_SOCKET;/*用于控制消息*/
28			pcmsghdr->cmsg_type = SCM_RIGHTS;
29			*((int*)CMSG_DATA(pcmsghdr))= sendfd;/*socket值*/
30			
31			
32			msghdr_send.msg_name = NULL;/*名称*/
33			msghdr_send.msg_namelen = 0;/*名称长度*/
34			
35			iov[0].iov_base = data;/*向量指针*/
36			iov[0].iov_len = bytes;/*数据长度*/
37			msghdr_send.msg_iov = iov;/*填充消息*/
38			msghdr_send.msg_iovlen = 1;
39			
40			return (sendmsg(fd, &msghdr_send, 0));/*发送消息*/
41		}
42		
43		
44		int main(int argc, char *argv[])
45		{
46			int fd;
47			ssize_t n;
48			
49			if(argc != 4){
50				printf("socketpair error\n");
51			if((fd = open(argv[2],atoi(argv[3])))<0)
52				return(0);
53				
54			if((n = put_fd(atoi(argv[1]),"",1,fd))<0)
55				return(0);
56		}