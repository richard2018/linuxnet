/* ping.c -- 第13章 例子 */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h> /* bzero */
#include <netdb.h>
#include <pthread.h>

static pingm_pakcet *icmp_findpacket(int seq);
static unsigned short icmp_cksum(unsigned char *data,  int len);
static struct timeval icmp_tvsub(struct timeval end,struct timeval begin);
static void icmp_statistics(void);
static void icmp_pack(struct icmp *icmph, int seq, struct timeval *tv, int length );
static int icmp_unpack(char *buf,int len);
static void *icmp_recv(void *argv);
static void * icmp_send(void *argv);
static void icmp_sigint(int signo);
static void icmp_usage();


/* 保存已经发送包的状态值 */
typedef struct pingm_pakcet{
	struct timeval tv_begin;	/*发送的时间*/
	struct timeval tv_end;		/*接收到的时间*/
	short seq;				/*序列号*/
	int flag;					/*1，表示已经发送但没有接收到回应包
								0，表示接收到回应包*/
}pingm_pakcet;
static pingm_pakcet pingpacket[128];

#define K 1024
#define BUFFERSIZE 72			/*发送缓冲区大小*/
static unsigned char send_buff[BUFFERSIZE];

static unsigned char recv_buff[2*K];	/*为防止接收溢出，接收缓冲区设置大一些*/
static struct sockaddr_in dest;		/*目的地址*/
static int rawsock = 0;				/*发送和接收线程需要的socket描述符*/
static pid_t pid=0;					/*进程PID*/
static int alive = 0;				/*是否接收到退出信号*/
static short packet_send = 0;		/*已经发送的数据包多少*/
static short packet_recv = 0;		/*已经接收的数据包多少*/
static char dest_str[80];			/*目的主机字符串*/
static struct timeval tv_begin, tv_end,tv_interval; /*本程序开始发送、结束和时间间隔*/

static void icmp_usage()
{
	/* ping加IP地址或者域名 */
	printf("ping aaa.bbb.ccc.ddd\n");
}

/* 终端信号处理函数SIGINT */
static void icmp_sigint(int signo)
{
	/* 告诉接收和发送线程结束程序 */
	alive = 0;
	/* 读取程序结束时间 */
	gettimeofday(&tv_end, NULL);
	/* 计算一下总共所用时间 */
	tv_interval = icmp_tvsub(tv_end, tv_begin);
	
	return;
}


/*查找一个合适的包位置
当seq为-1时，表示查找空包
其他值表示查找seq对应的包*/
static pingm_pakcet *icmp_findpacket(int seq)
{
	int i=0;
	pingm_pakcet *found = NULL;

	/* 查找包的位置 */
	if(seq == -1)/* 查找空的位置 */
	{
		for(i = 0;i<128;i++)
		{
			if(pingpacket[i].flag == 0)
			{
				found = &pingpacket[i];
				break;
			}
			
		}
	}
	else if(seq >= 0)/*查找对应SEQ的包*/
	{
		for(i = 0;i<128;i++)
		{
			if(pingpacket[i].seq == seq)
			{
				found = &pingpacket[i];
				break;
			}
			
		}
	}

	return found;
}

/* CRC16校验和计算icmp_cksum
参数：
	data:数据
	len:数据长度
返回值：
	计算结果，short类型
*/
static unsigned short icmp_cksum(unsigned char *data,  int len)
{
       int sum=0;/* 计算结果 */
	int odd = len & 0x01;/*是否为奇数*/

	unsigned short *value = (unsigned short*)data;
	/*将数据按照2字节为单位累加起来*/
       while( len & 0xfffe)  {
              sum += *(unsigned short*)data;
		data += 2;
		len -=2;
       }
	/*判断是否为奇数个数据，若ICMP报头为奇数个字节，会剩下最后一字节。*/
       if( odd) {
		unsigned short tmp = ((*data)<<8)&0xff00;
              sum += tmp;
       }
       sum = (sum >>16) + (sum & 0xffff);
       sum += (sum >>16) ;
       
       return ~sum;
}

/*计算时间差time_sub
参数：
	end,接收到的时间
	begin，开始发送的时间
返回值：
	使用的时间
*/
static struct timeval icmp_tvsub(struct timeval end,struct timeval begin)
{
	struct timeval tv;

	/*计算差值*/
	tv.tv_sec = end.tv_sec - begin.tv_sec;
	tv.tv_usec = end.tv_usec - begin.tv_usec;

	/* 如果接收时间的usec值小于发送时的usec值，从usec域借位 */
	if(tv.tv_usec < 0)
	{
		tv.tv_sec --;
		tv.tv_usec += 1000000; 
	}
	
	return tv;
}


/* 打印全部ICMP发送接收统计结果 */
static void icmp_statistics(void)
{       
	long time = (tv_interval.tv_sec * 1000 )+ (tv_interval.tv_usec/1000);
	printf("--- %s ping statistics ---\n",dest_str);/* 目的邋IP地址 */
	printf("%d packets transmitted, %d received, %d%c packet loss, time %d ms\n",
		packet_send,/* 发送 */
		packet_recv,  /* 接收 */
		(packet_send-packet_recv)*100/packet_send, /* 丢失百分比 */
		'%',
		time); /* 时间 */
}


/* 解压接收到的包，并打印信息 */
static int icmp_unpack(char *buf,int len)
{       
	int i,iphdrlen;
	struct ip *ip = NULL;
	struct icmp *icmp = NULL;
	int rtt;
	
	ip=(struct ip *)buf; 				/* IP头部 */
	iphdrlen=ip->ip_hl*4;				/* IP头部长度 */
	icmp=(struct icmp *)(buf+iphdrlen);	/* ICMP段的地址 */
	len-=iphdrlen;

	/* 判断长度是否为ICMP包 */
	if( len<8) 
	{       
		printf("ICMP packets\'s length is less than 8\n");
		return -1;
	}

	/* ICMP类型为ICMP_ECHOREPLY并且为本进程的PID */
	if( (icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id== pid) )	
	{
		struct timeval tv_internel,tv_recv,tv_send;
		/* 在发送表格中查找已经发送的包，按照seq */
		pingm_pakcet* packet = icmp_findpacket(icmp->icmp_seq);
		if(packet == NULL)
			return -1;

		/* 取消标志 */
		packet->flag = 0;
		/* 获取本包的发送时间 */
		tv_send = packet->tv_begin;
		
		/* 读取此时时间，计算时间差 */
		gettimeofday(&tv_recv, NULL);
		tv_internel = icmp_tvsub(tv_recv,tv_send);  
		
		rtt = tv_internel.tv_sec*1000+tv_internel.tv_usec/1000; 

		/* 打印结果，包含
		*  ICMP段长度
		*  源IP地址
		*  包的序列号
		*  TTL
		*  时间差
		*/
		printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%d ms\n",
			len,
			inet_ntoa(ip->ip_src),
			icmp->icmp_seq,
			ip->ip_ttl,
			rtt);

		/* 接收包数量加1 */
		packet_recv ++;
	}
	else
	{
		return -1;
	}
}



/*设置ICMP报头*/
static void icmp_pack(struct icmp *icmph, int seq, struct timeval *tv, int length )
{       
	unsigned char i = 0;

	/* 设置报头 */
	icmph->icmp_type = ICMP_ECHO;	/*ICMP回显请求*/
	icmph->icmp_code = 0;			/*code值为0*/
	icmph->icmp_cksum = 0;			/*先将cksum值填写0，便于之后的cksum计算*/
	icmph->icmp_seq = seq;			/*本报的序列号*/
	icmph->icmp_id = pid &0xffff;		/*填写PID*/

	for(i = 0; i< length; i++)
		icmph->icmp_data[i] = i;
	/* 计算校验和 */
	icmph->icmp_cksum = icmp_cksum((unsigned char*)icmph, length);
}


/* 发送ICMP回显请求包 */
static void * icmp_send(void *argv)
{
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 1;

	/* 保存程序开始发送数据的时间 */
	gettimeofday(&tv_begin, NULL);

	while(alive)
	{
		int size = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);/*当前包的发送时间*/
		
		icmp_pack((struct icmp *)send_buff, packet_send, &tv, 64 );/*打包数据*/
		size = sendto (rawsock,  send_buff, 64,  0,  /*发送给目的地址*/
			(struct sockaddr *)&dest, sizeof(dest) );
		if(size <0)
		{
			perror("sendto error");
			continue;
		}
		else
		{
			/* 在发送包状态数组中找一个空闲位置 */
			pingm_pakcet *packet = icmp_findpacket(-1);
			if(packet)
			{
				packet->seq = packet_send;			/* 设置seq */
				packet->flag = 1;						/* 已经使用 */
				gettimeofday( &packet->tv_begin, NULL);/* 发送时间 */
				packet_send ++;						/* 计数增加 */
			}
		}

		/* 每隔一秒，发送一个ICMP回显请求包 */
		sleep(1);		
	}
}


/* 接收ping目的主机的回复 */
static void *icmp_recv(void *argv)
{
	/* 轮训等待时间 */
	struct timeval tv;
	tv.tv_usec = 200;
	tv.tv_sec = 0;
	fd_set  readfd;

	/* 当没有信号发出一直接收数据 */
	while(alive)
	{
		int ret = 0;
		FD_ZERO(&readfd);
		FD_SET(rawsock, &readfd);
		ret = select(rawsock+1,&readfd, NULL, NULL, &tv);
		switch(ret)
		{
			case -1:
				/* 错误发生 */
				break;
			case 0:
				/* 超时 */
				break;
			default:
				{
					/* 收到一个包 */
					int fromlen = 0;
					struct sockaddr from;

					/* 接收数据 */
					int size = recv(rawsock, recv_buff,sizeof(recv_buff), 0) ;
					if(errno == EINTR)
					{
						perror("recvfrom error");
						continue;
					}

					/* 解包，并设置相关变量 */
					ret = icmp_unpack(recv_buff, size);
					if(ret == -1)
					{
						continue;
					}
				}
				break;
		}
		
	}
}



/* 主程序 */
int main(int argc, char *argv[])
{
	struct hostent * host = NULL;
	struct protoent *protocol = NULL;
	char protoname[]= "icmp";

	unsigned long inaddr = 1;
	int size = 128*K;

	/* 参数是否数量正确 */
	if(argc < 2)
	{
		icmp_usage();
		return -1;
	}

	/* 获取协议类型ICMP */
	protocol = getprotobyname(protoname);
	if (protocol == NULL)
	{
		perror("getprotobyname()");
		return -1;
	}

	/* 将目的地址字符串考出 */
	memcpy(dest_str,  argv[1], strlen(argv[1])+1);
	memset(pingpacket, 0, sizeof(pingm_pakcet) * 128);

	/* socket初始化 */
	rawsock = socket(AF_INET, SOCK_RAW,  protocol->p_proto);
	if(rawsock < 0)
	{
		perror("socket");
		return -1;
	}

	/* 为了与其他进程的ping程序区别，加入pid */

	pid = getuid();

	/* 增大接收缓冲区，防止接收的包被覆盖 */
	setsockopt(rawsock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	bzero(&dest, sizeof(dest));

	/* 获取目的地址的IP地址 */
	dest.sin_family = AF_INET;
	
	/* 输入的目的地址为字符串IP地址 */
	inaddr = inet_addr(argv[1]);
	if(inaddr == INADDR_NONE)
	{
		/* 为DNS地址 */
		host = gethostbyname(argv[1]);
		if(host == NULL)
		{
			perror("gethostbyname");
			return -1;
		}

		/* 将地址拷贝到dest中 */
		memcpy((char *)&dest.sin_addr, host->h_addr, host->h_length);
	}
	else/* 为IP地址字符串 */
	{
		memcpy((char*)&dest.sin_addr, &inaddr, sizeof(inaddr));
	}

	/* 打印提示 */
	inaddr = dest.sin_addr.s_addr;
	printf("PING %s (%d.%d.%d.%d) 56(84) bytes of data.\n", 
		dest_str, 
		(inaddr&0x000000FF)>>0,
		(inaddr&0x0000FF00)>>8,
		(inaddr&0x00FF0000)>>16,
		(inaddr&0xFF000000)>>24);

	/* 截取信号SIGINT,将icmp_sigint挂接上 */
	signal(SIGINT, icmp_sigint);
	
	/* 初始化为可运行 */
	alive = 1;
	/* 建立两个线程，用于发送和接收 */
	pthread_t send_id, recv_id;
	int err = 0;
	err = pthread_create(&send_id, NULL, icmp_send, NULL);/* 发送 */
	if(err < 0)
	{
		return -1;
	}

	err = pthread_create(&recv_id, NULL, icmp_recv, NULL);/* 接收 */
	if(err < 0)
	{
		return -1;
	}
	
	/* 等待线程结束 */
	pthread_join(send_id, NULL);
	pthread_join(recv_id, NULL);

	/* 清理并打印统计结果 */
	close(rawsock);
	icmp_statistics();

	return 0;	
}


