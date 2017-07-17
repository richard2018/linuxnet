#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <string.h>
#include <linux/sockios.h>

int main(int argc, char *argv[])
{
	int s;
	struct arpreq arpreq;
	struct sockaddr_in *addr = (struct sockaddr_in*)&arpreq.arp_pa;
	unsigned char *hw;

	int err = -1;

	if(argc < 2){
		printf("错误的使用方式,格式为:\nmyarp ip(myarp 127.0.0.1)\n");
		return -1;
	}

	/* 建立一个数据报套接字 */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)	{
		printf("socket() 出错\n");
		return -1;
	}

	/* 填充arpreq的成员arp_pa */
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(argv[1]);
	if(addr->sin_addr.s_addr == INADDR_NONE){
		printf("IP 地址格式错误\n");
	}
	/* 网络接口为eth0 */
	strcpy(arpreq.arp_dev, "eth0");

	err = ioctl(s, SIOCGARP, &arpreq);
	if(err < 0){/*失败*/
		printf("IOCTL 错误\n");
		return -1;
	}else{/*成功*/
		hw = (unsigned char*)&arpreq.arp_ha.sa_data;/*硬件地址*/
		printf("%s==>",argv[1]);/*打印IP*/
		printf("%02x:%02x:%02x:%02x:%02x:%02x\n",/*打印硬件地址*/
				hw[0],hw[1],hw[2],hw[3],hw[4],hw[5]);
	};

	close(s);
	return 0;
}
