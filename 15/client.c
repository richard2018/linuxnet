#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_LEN 1024	/*缓冲区长度*/
#define MYPORT 8888		/*服务器侦听端口*/
#define BACKLOG 10		/*服务器侦听长度*/
int main(int argc, char *argv[])
{
	int s_c;	/*客户端套接字文件描述符*/
	socklen_t len;
	int err = -1;
	
	struct sockaddr_in6 server_addr;	/*本地地址结构*/
	struct sockaddr_in6 client_addr;/*客户端地址结构*/
	char buf[BUF_LEN + 1];	/*收发缓冲区*/

    
    s_c = socket(PF_INET6, SOCK_STREAM, 0);/*建立IPv6套接字*/
    if (s_c == -1) { /*判断错误*/
        perror("socket error");
        return(1);
    } else{
        printf("socket() success\n");
	}
	
    bzero(&server_addr, sizeof(server_addr));/*清地址结构*/
    server_addr.sin6_family = PF_INET6;    /*协议族*/
    server_addr.sin6_port = htons(MYPORT); /*协议端口*/
    server_addr.sin6_addr = in6addr_loopback;   /*IPv6任意地址*/

	/*连接服务器*/
	err = connect(s_c, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (err == -1) {/*判断错误*/
        perror("connect error");
        return (1);
    } else{
        printf("connect() success\n");
	}
	
	/*清零缓冲区*/
	bzero(buf, BUF_LEN + 1);
	len = recv(s_c, buf, BUF_LEN, 0);/*接收服务器数据*/
	/*打印信息*/
	printf("RECVED %dbytes:%s\n",len,buf);
	bzero(buf, BUF_LEN + 1);/*清零缓冲区*/
    strcpy(buf,"From Client");/*拷贝客户端的信息*/
    len = send(s_c, buf, strlen(buf), 0);/*向服务器发送数据*/
    close(s_c);/*关闭套接字*/
    
	
    return 0;
}
