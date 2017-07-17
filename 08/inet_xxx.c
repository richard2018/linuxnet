#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct in_addr ip,local,network;
	char addr1[]="192.168.1.1";		/* 网络地址字符串 */
	char addr2[]="255.255.255.255";
	char addr3[]="192.16.1";
	char addr[16];
	char *str=NULL,*str2=NULL;

	
	int err = 0;
	
	/* 测试函数inet_aton */	
	err = inet_aton(addr1, &ip);
	if(err){
		printf("inet_aton:string %s value is:0x%x\n",addr1, ip.s_addr);	
	}else{
		printf("inet_aton:string %s  error\n",addr1);	
	}
	
	/* inet_addr，先测试192.168.1.1，在测试255.255.255.255 */	
	ip.s_addr = inet_addr(addr1);
	if(err != -1){
		printf("inet_addr:string %s value is:0x%x\n",addr1, ip.s_addr);	
	}else{
		printf("inet_addr:string %s  error\n",addr1);
	};
	ip.s_addr = inet_addr(addr2);
	if(ip.s_addr != -1){
		printf("inet_addr:string %s value is:0x%x\n",addr2, ip.s_addr);	
	}else{
		printf("inet_addr:string %s  error\n",addr2);
	};
	
	/* inet_ntoa，先测试192.168.1.1，在测试255.255.255.255
	*	证明函数的不可重入性
	 */
	ip.s_addr = 192<<24|168<<16|1<<8|1;
	str = inet_ntoa(ip);
	ip.s_addr = 255<<24|255<<16|255<<8|255;
	str2 = inet_ntoa(ip);	
	printf("inet_ntoa:ip:0x%x string1 %s,pre is:%s \n",ip.s_addr,str2,str);
	
	/* 测试函数inet_addr */	
	ip.s_addr = inet_addr(addr3);
	if(err != -1){
		printf("inet_addr:string %s value is:0x%x\n",addr3, ip.s_addr);	
	}else{
		printf("inet_addr:string %s  error\n",addr3);
	};
	str = inet_ntoa(ip);	
	printf("inet_ntoa:string %s ip:0x%x \n",str,ip.s_addr);
	
	/* 测试函数inet_lnaof，获得本机地址 */	
	inet_aton(addr1, &ip);
	local.s_addr = htonl(ip.s_addr);
	local.s_addr = inet_lnaof(ip);
	str = inet_ntoa(local);	
	printf("inet_lnaof:string %s ip:0x%x \n",str,local.s_addr);
	
	/* 测试函数inet_netof，获得本机地址 */
	network.s_addr = inet_netof(ip);
	printf("inet_netof:value:0x%x \n",network.s_addr);
	
	
	return 0;	
}
