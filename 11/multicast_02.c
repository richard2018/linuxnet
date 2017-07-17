/*

* listen.c - An IP multicast client */
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

/* 多播IP端口 */
#define MCAST_PORT 6789;
/* 多播IP地址 */
#define MCAST_ADDR "224.1.1.1"
int main(void)
{
	struct ip_mreq mreq;
	int loop = 1;
	/* 多播循环 */
	int iter = 0;
	int sin_len;
	char message[256];
	int s;
	struct sockaddr_in local_addr;
	struct hostent *host;
	
	host = gethostbyname(host_name);
	if(host == 0)
	{
		perror("gethostbyname");
		exit(EXIT_FAILURE);
	}
	
	/*bzero(&local_addr, sizeof(local_addr));*/
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(port);
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if(s == -1)
	{ 
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	/* 调用bind之前，设置套接口选项启用多播IP支持*/
	loop = 1;
	int err = setsockopt(s,SOL_SOCKET, SO_REUSEADDR,&loop, sizeof(loop));
	if( err< 0)
	{
		perror("setsockopt:SO_REUSEADDR");
		exit(EXIT_FAILURE);
	}
	
	err = bind(s,(struct sockaddr *)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	/* 在同一个主机上进行广播设置套接口，
	作用是方便单个开发系统上测试多播IP广播 */
	loop = 1;
	err = setsockopt(s,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
	if(err < 0)
	{
		perror("setsockopt:IP_MULTICAST_LOOP");
		exit(EXIT_FAILURE);
	}
	
	/* 加入一个广播组。进一步告诉Linux内核，
	特定的套接口即将接受广播数据*/
	mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if(mreq.imr_multiaddr.s_addr == -1)
	{
		perror("224.0.0.1 not a legal multicast address");
		exit(EXIT_FAILURE);
	}
	
	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
	if (err < 0)
	{
		perror("setsockopt:IP_ADD_MEMBERSHIP");
	}
	
	while(iter++ < 8)
	{
		int n = -1;
		sin_len = sizeof(local_addr);
		n = recvfrom(s, message, 256, 0,(struct sockaddr *)&local_addr, &sin_len);
		if( n== -1) 
		{
			perror("recvfrom");
		}
		printf("Response #%-2d from server: %s\n", iter, message);
		sleep(2); 
	}
	
	/* 接受8个广播后退出 */
	err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	if(err < 0) 
	{
		perror("setsockopt:IP_DROP_MEMBERSHIP");
	}
	
	close(s);
	exit(EXIT_SUCCESS);
}
