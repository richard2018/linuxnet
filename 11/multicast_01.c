/*
* broadcast.c - An IP multicast server
*/
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

#define MCAST_PORT 6789;
#define MCAST_ADDR "224.1.1.1"
int main(void)
{
	int s;
	struct sockaddr_in mcast_addr;
	
	/* 首先建立套接口 */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		perror("Opening socket");
		exit(EXIT_FAILURE);
	}
	
	/* 初始化IP多播地址 */
	memset(&mcast_addr, 0, sizeof(mcast_addr));
	mcast_addr.sin_family = AF_INET;
	mcast_addr.sin_addr.s_addr = inet_addr("224.0.0.1");
	mcast_addr.sin_port = htons(MCAST_PORT);
	/* 开始进行IP多播 */
	while(1) {
		int n = sendto(s, 
				"test from broadcast",
				sizeof("test from broadcast"), 
				0,
				(struct sockaddr *)&mcast_addr, 
				sizeof(mcast_addr)) ;
		if( n < 0)
		{
			perror("sendto");
			exit(EXIT_FAILURE);
		}
		sleep(2);
	}
	exit(EXIT_SUCCESS);
}
