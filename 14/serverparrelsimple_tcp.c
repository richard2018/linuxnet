#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
#define PIDNUMB 2
static void handle_connect(int s_s)
{	
	int s_c;	/*客户端套接字文件描述符*/
	struct sockaddr_in from;	/*客户端地址*/
	int len = sizeof(from);
	
	/*主处理过程*/
	while(1)
	{
		/*接收客户端连接*/
		s_c = accept(s_s, (struct sockaddr*)&from, &len);
		time_t now;		/*时间*/
		char buff[BUFFLEN];/*收发数据缓冲区*/
		int n = 0;
		memset(buff, 0, BUFFLEN);/*清零*/
		n = recv(s_c, buff, BUFFLEN,0);/*接收发送方数据*/
		if(n > 0 && !strncmp(buff, "TIME", 4))/*判断是否合法接收数据*/
		{
			memset(buff, 0, BUFFLEN);/*清零*/
			now = time(NULL);/*当前时间*/
			sprintf(buff, "%24s\r\n",ctime(&now));/*将时间拷贝入缓冲区*/
			send(s_c, buff, strlen(buff),0);/*发送数据*/
		}		
		/*关闭客户端*/
		close(s_c);	
	}
		
}

void sig_int(int num)
{
	exit(1);	
}
int main(int argc, char *argv[])
{
	int s_s;	/*服务器套接字文件描述符*/
	struct sockaddr_in local;	/*本地地址*/	
	signal(SIGINT,sig_int);
	
	/*建立TCP套接字*/
	s_s = socket(AF_INET, SOCK_STREAM, 0);
	
	/*初始化地址接哦股*/
	memset(&local, 0, sizeof(local));/*清零*/
	local.sin_family = AF_INET;/*AF_INET协议族*/
	local.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
	local.sin_port = htons(SERVER_PORT);/*服务器端口*/
	
	/*将套接字文件描述符绑定到本地地址和端口*/
	int err = bind(s_s, (struct sockaddr*)&local, sizeof(local));
	err = listen(s_s, BACKLOG);/*侦听*/
	
	/*处理客户端连接*/
	pid_t pid[PIDNUMB];
	int i =0;
	for(i=0;i<PIDNUMB;i++)
	{
		pid[i] = fork();
		if(pid[i] == 0)/*子进程*/
		{
			handle_connect(s_s);
		}
	}
	while(1);
	
	close(s_s);
	
	return 0;		
}
