#include <sys/types.h>
#include <sys/socket.h>	/*socket()/bind()*/
#include <linux/in.h>	/*struct sockaddr_in*/
#include <string.h>		/*memset()*/
#include <stdio.h>

#define PORT_SERV 8888
#define BUFF_LEN 256
static void udpclie_echo(int s, struct sockaddr *to)
{
	char buff[BUFF_LEN] = "UDP TEST";
	struct sockaddr_in from;
	int len = sizeof(*to);
	sendto(s, buff, BUFF_LEN, 0, to, len);
	int i = 0;
	for(i = 0; i< 4; i++)
	{
		memset(buff, 0, BUFF_LEN);
		int err = recvfrom(s, buff, 1, 0, (struct sockaddr*)&from, &len);
		printf("%dst:%c,err:%d\n",i,buff[0],err);
	}
	printf("recved:%s\n",buff);	
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

