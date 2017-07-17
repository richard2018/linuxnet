#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define ADDRLEN 16
int main(int argc, char *argv[])
{
	struct in_addr ip;
	char IPSTR[]="192.168.1.1";		/* 网络地址字符串 */
	char addr[ADDRLEN];				/* 保存转换后的字符串IP地址，16个字节大小 */
	const char*str=NULL;	
	int err = 0;					/* 返回值 */
	
	/* 测试函数inet_pton转换192.168.1.1为二进制形式 */	
	err = inet_pton(AF_INET, IPSTR, &ip);	/* 将字符串转换为二进制 */
	if(err > 0){
		printf("inet_pton:ip,%s value is:0x%x\n",IPSTR,ip.s_addr);
	}
	
	/* 测试函数inet_ntop转换192.168.1.1为字符串 */
	ip.s_addr = htonl(192<<24|168<<16|12<<8|255);/*192.168.12.255*/
	/*将二进制网络字节序192.168.12.255转换为字符串*/
	str = (const char*)inet_ntop(AF_INET, (void*)&ip, (char*)&addr[0], ADDRLEN);
	if(str){
		printf("inet_ntop:ip,0x%x is %s\n",ip.s_addr,str);	
	}
	
	return 0;	
}
