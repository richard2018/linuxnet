#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <linux/in.h>

/* 结构保存获取结果 */
typedef union optval {
	int 			val;		/*整型值*/
	struct linger 	linger;		/*linger结构*/
	struct timeval 	tv;			/*时间结构*/
	unsigned char 	str[16];	/*字符串*/
}val;

/*数值类型*/
typedef enum valtype{
	VALINT,		/*int类型*/
	VALLINGER,	/*struct linger类型*/
	VALTIMEVAL,	/*struct timeval类型*/
	VALUCHAR,	/*字符串*/
	VALMAX		/*错误类型*/
}valtype;

/* 用于保存套接字选项的结构 */
typedef struct sopts{
	int level;		/*套接字选项级别*/
	int optname;	/*套接字选项名称*/
	char *name;		/*套接字名称*/
	valtype valtype;/*套接字返回参数类型*/
}sopts;

static val optval;/*用于保存数值*/
sopts sockopts[] = {
	{SOL_SOCKET, 	SO_BROADCAST,		"SO_BROADCAST",			VALINT},
	{SOL_SOCKET, 	SO_DEBUG,			"SO_DEBUG",				VALINT},
	{SOL_SOCKET, 	SO_DONTROUTE,		"SO_DONTROUTE",			VALINT},
	{SOL_SOCKET, 	SO_ERROR,			"SO_ERROR",				VALINT},
	{SOL_SOCKET, 	SO_KEEPALIVE,		"SO_KEEPALIVE",			VALINT},
	{SOL_SOCKET, 	SO_LINGER,			"SO_LINGER",			VALINT},
	{SOL_SOCKET, 	SO_OOBINLINE,		"SO_OOBINLINE",			VALINT},
	{SOL_SOCKET, 	SO_RCVBUF,			"SO_RCVBUF",			VALINT},
	{SOL_SOCKET, 	SO_RCVLOWAT,		"SO_RCVLOWAT",			VALINT},
	{SOL_SOCKET, 	SO_RCVTIMEO,		"SO_RCVTIMEO",			VALTIMEVAL},
	{SOL_SOCKET, 	SO_SNDTIMEO,		"SO_SNDTIMEO",			VALTIMEVAL},
	{SOL_SOCKET, 	SO_TYPE,			"SO_TYPE",				VALINT},
	{IPPROTO_IP, 	IP_HDRINCL,			"IP_HDRINCL",			VALINT},
	{IPPROTO_IP, 	IP_OPTIONS,			"IP_OPTIONS",			VALINT},
	{IPPROTO_IP, 	IP_TOS,				"IP_TOS",				VALINT},
	{IPPROTO_IP, 	IP_TTL,				"IP_TTL",				VALINT},
	{IPPROTO_IP, 	IP_MULTICAST_TTL,	"IP_MULTICAST_TTL",		VALUCHAR},
	{IPPROTO_IP, 	IP_MULTICAST_LOOP,	"IP_MULTICAST_LOOP",	VALUCHAR},
	{IPPROTO_TCP, 	TCP_KEEPCNT,		"TCP_KEEPCNT",		VALINT},
	{IPPROTO_TCP, 	TCP_MAXSEG,			"TCP_MAXSEG",			VALINT},
	{IPPROTO_TCP, 	TCP_NODELAY,		"TCP_NODELAY",			VALINT},
	{0,				0,					NULL,					VALMAX}/*结尾，主程序中判断VALMAX*/
};

/* 显示查询结果 */
static void disp_outcome(sopts *sockopt, int len, int err)
{
	if(err == -1){/* 错误 */
		printf("optname %s NOT support\n",sockopt->name);
		return;	
	}
	
	switch(sockopt->valtype){/*根据不同的类型进行信息打印*/
		case VALINT:/*整型*/
			printf("optname %s: default is %d\n",sockopt->name,optval.val);
			break;
		case VALLINGER:/*struct linger*/
			printf("optname %s: default is  %d(ON/OFF), %d to linger\n",
				sockopt->name,			/*名称*/
				optval.linger.l_onoff,/*linger打开*/
				optval.linger.l_linger);/*延时时间*/
			break;
		case VALTIMEVAL:/*struct timeval结构*/
			printf("optname %s: default is %.06f\n",
				sockopt->name,/*名称*/
				((((double)optval.tv.tv_sec*100000+(double)optval.tv.tv_usec))/(double)1000000));/*浮点型结构*/
			break;
		case VALUCHAR:/*字符串类型，循环打印*/
			{
				int i = 0;
				printf("optname %s: default is ",sockopt->name);/*选项名称*/
				for(i = 0; i < len; i++){
					printf("%02x ", optval.str[i]);			
				}
				printf("\n");					
			}
			break;
		default:
			break;
	}	
}

int main(int argc, char *argv[])
{
	int err = -1;
	int len = 0;
	int i = 0;
	int s = socket(AF_INET, SOCK_STREAM, 0);/*建立一个流式套接字*/
	while(sockopts[i].valtype != VALMAX){/*判断是否结尾，否则轮询执行*/
		len = sizeof(sopts);/*计算结构长度*/
		err = getsockopt(s, sockopts->level, sockopts->optname, &optval, &len);/*获取选项状态*/
		
		disp_outcome(&sockopts[i], len, err);/*显示结果*/
		i++;/*递增*/
	}

	close(s);
	return 0;	
}
