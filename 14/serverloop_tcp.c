#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
int main(int argc, char *argv[])
{
	int s_s, s_c;	/*服务器套接字文件描述符*/
	struct sockaddr_in local, from;	/*本地地址*/
	time_t now;		/*时间*/
	char buff[BUFFLEN];/*收发数据缓冲区*/
	int n = 0;
	int len = sizeof(from);
	
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
	
	/*主处理过程*/
	while(1)
	{
		/*接收客户端连接*/
		s_c = accept(s_s, (struct sockaddr*)&from, &len);
		memset(buff, 0, BUFFLEN);/*清零*/
		n = recv(s_c, buff, BUFFLEN,0);/*接收发送方数据*/
		if(n > 0 && !strncmp(buff, "TIME", 4))/*判断是否合法接收数据*/
		{
			memset(buff, 0, BUFFLEN);/*清零*/
			now = time(NULL);/*当前时间*/
			sprintf(buff, "%24s\r\n",ctime(&now));/*将时间拷贝入缓冲区*/
			send(s_c, buff, strlen(buff),0);/*发送数据*/
		}
		close(s_c);
	}
	close(s_s);
	
	return 0;		
}
