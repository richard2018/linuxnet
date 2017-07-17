#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#define BUFFLEN 1024
#define SERVER_PORT 8888
int main(int argc, char *argv[])
{
	int s;	/*服务器套接字文件描述符*/
	struct sockaddr_in local, to;	/*本地地址*/
	time_t now;		/*时间*/
	char buff[BUFFLEN];/*收发数据缓冲区*/
	int n = 0;
	int len = sizeof(to);
	
	/*建立UDP套接字*/
	s = socket(AF_INET, SOCK_DGRAM, 0);
	
	/*初始化地址接哦股*/
	memset(&local, 0, sizeof(local));/*清零*/
	local.sin_family = AF_INET;/*AF_INET协议族*/
	local.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
	local.sin_port = htons(SERVER_PORT);/*服务器端口*/
	
	/*将套接字文件描述符绑定到本地地址和端口*/
	int err = bind(s, (struct sockaddr*)&local, sizeof(local));
	/*主处理过程*/
	while(1)
	{
		memset(buff, 0, BUFFLEN);/*清零*/
		n = recvfrom(s, buff, BUFFLEN,0,(struct sockaddr*)&to, &len);/*接收发送方数据*/
		if(n > 0 && !strncmp(buff, "TIME", 4))/*判断是否合法接收数据*/
		{
			memset(buff, 0, BUFFLEN);/*清零*/
			now = time(NULL);/*当前时间*/
			sprintf(buff, "%24s\r\n",ctime(&now));/*将时间拷贝入缓冲区*/
			sendto(s, buff, strlen(buff),0, (struct sockaddr*)&to, len);/*发送数据*/
		}
	}
	close(s);
	
	return 0;		
}
