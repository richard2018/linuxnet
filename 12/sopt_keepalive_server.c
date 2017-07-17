#include<stdio.h> 
#include<stdlib.h> 
#include<sys/socket.h> 
#include<error.h> 
#include<string.h> 
#include<sys/types.h> 
#include<netinet/in.h> 
#include<sys/wait.h> 
#include<arpa/inet.h> 
#include<unistd.h> 

/* 用于处理SIGPIP和SIGINT信号的函数 */
static int sigpipe(int signo)
{
	alive = 0;
}

#define PORT 8888 		/* 服务器侦听端口为8888 */
#define BACKLOG 8 		/* 最大侦听排队数量为8 */
static int alive = 1;	/* 是否退出 */
int main(int argc, char *argv[])
{
	/* s为服务器的侦听套接字描述符，sc为客户端连接成功返回的描述符 */
	int s, sc; 
	/* local_addr本地地址，client_addr客户端的地址 */
	struct sockaddr_in local_addr,client_addr; 
	int err = -1; 	/* 错误返回值 */
	socket_t optlen = -1;	/* 整型的选项类型值 */
	int optval = -1;		/* 选项类型值长度 */
	
	/* 截取SIGPIPE和SIGINT由函数signo处理 */
	signal(SIGPIPE, signo);
	signal(SIGINT,signo);
	
	/* 创建本地监听套接字 */
	s = socket(AF_INET,SOCK_STREAM,0);
	if( s == -1){ 
		printf("套接字创建失败!\n"); 
		return -1;
	} 
	
	/* 设置地址和端口重用 */
	optval = 1; /* 重用有效 */
	optlen = sizeof(optval);	
	err=setsockopt(s, SOL_SOCKET, SO_REUSEADDR,(char *)&optval, optlen); 
	if(err!= -1){ /* 设置失败 */
		printf("套接字可重用设置失败!\n"); 
		return -1; 
	}
	
	/* 初始化本地协议族，端口和IP地址 */
	bzero(&local_addr, 0, sizeof(local_addr));	/* 清理 */
	local_addr.sin_family=AF_INET; 				/* 协议族 */
	local_addr.sin_port=htons(PORT); 			/* 端口 */
	local_addr.sin_addr.s_addr=INADDR_ANY; 		/* 任意本地地址 */
	
	/* 绑定套接字 */
	err = bind(s, (struct sockaddr *)&local_addr, sizeof(struct sockaddr);
	if(err == -1){ /* 绑定失败 */
		printf("绑定失败!\n"); 
		return -1;
	} 
	
	/* 设置最大接收缓冲区和最大发送缓冲区 */
	optval = 128*1024; /* 缓冲区大小为128K */
	optlen = sizeof(optval);
	err = setsockopt(s, SOL_SOCKET, SO_RCVBUF, &optval, optlen);
	if(err == -1){/* 设置接收缓冲区大小失败 */
		printf("设置接收缓冲区失败\n");			
	}
	err = setsockopt(s, SOL_SOCKET, SO_SNDBUF, &optval, optlen);
	if(err == -1){/* 设置发送缓冲区大小失败 */
		printf("设置发送缓冲区失败\n");			
	}
	
	/* 设置发送和接收超时时间 */
	struct timeval tv;
	tv.tv_sec = 1;	/* 1秒 */
	tv.tv_usec = 200000;/* 200ms */
	optlen = sizeof(tv);
	err = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, optlen); /* 设置接收超时时间 */
	if(err == -1){/* 设置接收超时时间失败 */
		printf("设置接收超时时间失败\n");			
	}
	err = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, optlen);/* 设置发送超时时间 */
	if(err == -1){
		printf("设置发送超时时间失败\n");			
	}
	
		
	/* 设置监听 */
	err = listen(s,BACKLOG);
	if( err ==-1){ /* 设置监听失败 */
		printf("设置监听失败!\n"); 
		return -1; 
	} 
	
	printf("等待连接...\n"); 	
	fd_set fd_r;			/* 读文件描述符集 */
	struct timeval tv;		
	tv.tv_usec = 200000;	/* 超时时间为200ms */
	tv.tv_sec = 0;	
	while(alive){ 
		//有连接请求时进行连接 
		socklen_t sin_size=sizeof(struct sockaddr_in); 
		
		/* 此处每次会轮询是否有客户端连接到来，间隔时间为200ms */
		FD_ZERO(&fd_r);		/* 清除文件描述符集 */
		FD_SET(s, &fd_r);	/* 将侦听描述符放入 */
		switch (select(s + 1, &fd_r, NULL, &tv)) {  /* 监视文件描述符集fd_r */
                case -1:  	/* 错误发生 */                        
                case 0: 	/* 超时 */
                	continue;  
                	break;
                default: 	/* 有连接到来 */
                	break;                         
        }  
		/* 有连接到来，接收... */
		sc = accept(s, (struct sockaddr *)&client_addr,&sin_size);
		if( sc ==-1){ /* 失败 */
			perror("接受连接失败!\n"); 
			continue; 
		} 
	
		/* 设置连接探测超时时间 */
		optval = 10;			/* 10秒 */
		optlen = sizeof(optval);/**/
		err = setsockopt(sc, IPPROTO_TCP, SO_KEEPALIVE, (char*)&optval, optlen);/* 设置... */
		if( err == -1){/* 失败 */
			printf("设置连接探测间隔时间失败\n");	
		}
		
		/* 设置禁止Nagle算法 */
		optval = 1;					/* 禁止 */
		optlen = sizeof(optval);
		err = setsockopt(sc, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, optlen);/* 设置... */
		if( err == -1){/* 失败 */
			printf("禁止Nagle算法失败\n");	
		}
		
		/* 设置连接延迟关闭为立即关闭 */
		struct linger;
		linger.l_onoff = 1;		/* 延迟关闭生效 */
		linger.l_linger = 0;	/* 立即关闭 */
		optlen = sizeof(linger);
		err = setsockopt(sc, SOL_SOCKET, SO_LINGER, (char*)&linger, optlen);/* 设置... */
		if( err == -1){/* 失败 */
			printf("设置立即关闭失败\n");	
		}	
		
		/* 打印客户端IP地址信息 */
		printf("接到一个来自%s的连接\n",inet_ntoa(client_addr.sin_addr)); 	
		err = send(sc,"连接成功!\n",10,0);
		if(err == -1){
			printf("发送通知信息失败!\n"); 			 
		} 
		
		/*  关闭客户端连接 */
		close(sc);
		
	} 
	/* 关闭服务器端 */
	close(s); 
	
	return 0; 
} 
