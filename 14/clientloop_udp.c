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
	struct sockaddr_in server;	/*本地地址*/
	time_t now;		/*时间*/
	char buff[BUFFLEN];/*收发数据缓冲区*/
	int n = 0;	/*接收字符串长度*/
	int len = 0;	/*地址长度*/
	
	/*建立UDP套接字*/
	s = socket(AF_INET, SOCK_DGRAM, 0);
	
	/*初始化地址接哦股*/
	memset(&server, 0, sizeof(server));/*清零*/
	server.sin_family = AF_INET;/*AF_INET协议族*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
	server.sin_port = htons(SERVER_PORT);/*服务器端口*/	
	
	memset(buff, 0, BUFFLEN);/*清零*/
	strcpy(buff, "TIME");/*拷贝发送字符串*/
	/*发送数据*/
	sendto(s, buff, strlen(buff), 0, (struct sockaddr*)&server, sizeof(server));
	memset(buff, 0, BUFFLEN);/*清零*/
	/*接收数据*/
	len = sizeof(server);
	n = recvfrom(s, buff, BUFFLEN, 0, (struct sockaddr*)&server, &len);
	/*打印消息*/
	if(n >0){
		printf("TIME:%s",buff);	
	}
	close(s);
	
	return 0;		
}
