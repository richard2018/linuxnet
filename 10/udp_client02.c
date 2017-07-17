#include <sys/types.h>
#include <sys/socket.h>	/*socket()/bind()*/
#include <linux/in.h>	/*struct sockaddr_in*/
#include <string.h>		/*memset()*/
#include <stdio.h>

#define PORT_SERV 8888
#define BUFF_LEN 256


#define PORT_SERV 8888		/*服务器端口*/
#define NUM_DATA 100		/*接收缓冲区数量*/
#define LENGTH 1024			/*单个接收缓冲区大小*/
static char buff_send[LENGTH]; /*接收缓冲区*/
static void udpclie_echo(int s, struct sockaddr *to)
{
	char buff_init[BUFF_LEN] = "UDP TEST";	/*向服务器端发送的数据*/
	struct sockaddr_in from;/*发送数据的主机地址*/
	int len = sizeof(*to);/*地址长度*/
	int i = 0;/*计数*/
	for(i = 0; i< NUM_DATA; i++)/*循环发送*/
	{
		*((int*)&buff_send[0]) = htonl(i);/*将数据标记打包*/
		memcpy(&buff_send[4],buff_init, sizeof(buff_init));/*数据考入发送缓冲区*/
		sendto(s, &buff_send[0], NUM_DATA, 0, to, len);/*发送数据*/
	}
}
int main(int argc, char *argv[])
{
	int s;
	struct sockaddr_in addr_serv;	
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_serv.sin_port = htons(PORT_SERV);
	
	udpclie_echo(s, (struct sockaddr*)&addr_serv);
	
	close(s);
	return 0;	
}

