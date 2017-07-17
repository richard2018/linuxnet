#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888
int main(int argc, char *argv[])
{
	int s;	/*服务器套接字文件描述符*/
	struct sockaddr_in server;	/*本地地址*/
	char buff[BUFFLEN];/*收发数据缓冲区*/
	int n = 0;	/*接收字符串长度*/
	
	/*建立TCP套接字*/
	s = socket(AF_INET, SOCK_STREAM, 0);
	
	/*初始化地址接哦股*/
	memset(&server, 0, sizeof(server));/*清零*/
	server.sin_family = AF_INET;/*AF_INET协议族*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
	server.sin_port = htons(SERVER_PORT);/*服务器端口*/	
	
	/*连接服务器*/
	int err = connect(s, (struct sockaddr*)&server,sizeof(server));
	memset(buff, 0, BUFFLEN);/*清零*/
	strcpy(buff, "TIME");/*拷贝发送字符串*/
	/*发送数据*/
	send(s, buff, strlen(buff), 0);
	memset(buff, 0, BUFFLEN);/*清零*/
	/*接收数据*/	
	n = recv(s, buff, BUFFLEN, 0);
	/*打印消息*/
	if(n >0){
		printf("TIME:%s",buff);	
	}
	close(s);
	
	return 0;		
}
