#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <signal.h>

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
static int s;
void sig_proccess_client(int signo)
{
	printf("Catch a exit signal\n");
	close(s);
	exit(0);	
}

#define PORT 8888	/* 侦听端口地址 */
int main(int argc, char *argv[])
{

	struct sockaddr_in server_addr;	/* 服务器地址结构 */
	int err;/* 返回值 */
		
	if(argc == 1){
		printf("PLS input server addr\n");
		return 0;	
	}
	signal(SIGINT, sig_proccess);
	signal(SIGPIPE, sig_pipe);
	
	/* 建立一个流式套接字 */
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0){/* 出错 */
		printf("socket error\n");
		return -1;	
	}	
	
	/* 设置服务器地址 */
	bzero(&server_addr, sizeof(server_addr));		/* 清0 */
	server_addr.sin_family = AF_INET;				/* 协议族 */
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/* 本地地址 */
	server_addr.sin_port = htons(PORT);				/* 服务器端口 */
	
	/* 将用户输入的字符串类型的IP地址转为整型 */
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);	
	/* 连接服务器 */
	connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	process_conn_client(s);	/* 客户端处理过程 */
	close(s);	/* 关闭连接 */
}

