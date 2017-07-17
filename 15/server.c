#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_LEN 1024	/*缓冲区长度*/
#define MYPORT 8888		/*服务器侦听端口*/
#define BACKLOG 10		/*服务器侦听长度*/
int main(int argc, char **argv)
{
	int s_s;	/*服务器端套接字文件描述符*/
	int s_c;	/*客户端套接字文件描述符*/
	socklen_t len;
	int err = -1;
	
	struct sockaddr_in6 local_addr;	/*本地地址结构*/
	struct sockaddr_in6 client_addr;/*客户端地址结构*/
	char buf[BUF_LEN + 1];

    
    s_s = socket(PF_INET6, SOCK_STREAM, 0);/*建立IPv6套接字*/
    if (s_s == -1) { /*判断错误*/
        perror("socket error");
        return(1);
    } else{
        printf("socket() success\n");
	}
	
    bzero(&local_addr, sizeof(local_addr));/*清地址结构*/
    local_addr.sin6_family = PF_INET6;    /*协议族*/
    local_addr.sin6_port = htons(MYPORT); /*协议端口*/
    local_addr.sin6_addr = in6addr_any;   /*IPv6任意地址*/

	err = bind(s_s, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_in6));
	if (err == -1) {/*判断错误*/
        perror("bind error");
        return (1);
    } else{
        printf("bind() success\n");
	}
	
	err = listen(s_s, BACKLOG);/*设置侦听队列*/
    if (err == -1) {/*判断错误*/
        perror("listen error");
        exit(1);
    } else{
        printf("listen() success\n");
	}
	
    while (1) 
    {
        len = sizeof(struct sockaddr);/*地址长度*/
        /*等待客户端连接*/
        s_c = accept(s_s, (struct sockaddr *)&client_addr, &len);
        if (s_c == -1) {/*判断错误*/
            perror("accept error");
            return (errno);
        } else{
        	/*将客户端的地址转换成字符串*/
        	inet_ntop(AF_INET6, &client_addr.sin6_addr, buf, sizeof(buf));
            printf("a client from ip: %s, port %d, socket %d\n",
                   buf, /*客户端地址*/
                   client_addr.sin6_port,/*客户端端口*/ 
                   s_c); /*客户端套接字文件描述符*/
		}
		
        /* 开始处理每个新连接上的数据收发 */
        bzero(buf, BUF_LEN + 1);/*清零缓冲区*/
        strcpy(buf,"From Server");/*拷贝数据*/
        /* 发消息给客户端 */
        len = send(s_c, buf, strlen(buf), 0);
        if (len < 0) {/*错误信息*/
            printf("message '%s' send error,errno:%d,'%s'\n",
                 buf, errno, strerror(errno));
        } else{/*成功信息*/
            printf("message '%s' send success, %dbytes\n",buf, len);
		}
		/*清零缓冲区*/
        bzero(buf, BUF_LEN + 1);
        /* 接收客户端的消息 */
        len = recv(s_c, buf, BUF_LEN, 0);
        if (len > 0)
            printf("recv message success:'%s',%dbytes\n",
                   buf, len);
        else
            printf
                ("recv message failure, errno: %d，'%s'\n",
                 errno, strerror(errno));
        /* 处理每个新连接上的数据收发结束 */
        close(s_c);
    }

    close(s_s);
    return 0;
}
