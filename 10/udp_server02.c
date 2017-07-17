#include <sys/types.h>
#include <sys/socket.h>	/*socket()/bind()*/
#include <linux/in.h>	/*struct sockaddr_in*/
#include <string.h>		/*memset()*/

#define PORT_SERV 8888		/*服务器端口*/
#define NUM_DATA 100		/*接收缓冲区数量*/
#define LENGTH 1024			/*单个接收缓冲区大小*/
static char buff[NUM_DATA][LENGTH]; /*接收缓冲区*/
static udpserv_echo(int s, struct sockaddr *client)
{
	int n;	/*接收数量*/
	char tmp_buff[LENGTH];	/*临时缓冲区*/
	int len;	/*地址长度*/
	while(1)/*接收过程*/
	{
		len = sizeof(*client);/*地址长度*/
		n = recvfrom(s, tmp_buff, LENGTH, 0, client, &len);/*接收数据放到临时缓冲区中*/
		/*根据接收到数据的头部标志，选择合适的缓冲区位置拷贝数据*/
		memcpy(&buff[ntohl(*((int*)&buff[i][0]))][0], tmp_buff+4, n-4); 
	}	
}

int main(int argc, char *argv[])
{
	int s;
	struct sockaddr_in addr_serv,addr_clie;
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_serv.sin_port = htons(PORT_SERV);
	
	bind(s, (struct sockaddr*)&addr_serv, sizeof(addr_serv));
	udpserv_echo(s, (struct sockaddr*)&addr_clie);
	
	return 0;	
}
