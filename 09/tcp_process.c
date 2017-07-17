#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define RECVMSGSENDMSG

#ifdef RECVSEND


/* 服务器对客户端的处理 */
void process_conn_server(int s)
{
	ssize_t size = 0;
	char buffer[1024];	/* 数据的缓冲区 */
	
	for(;;){/* 循环处理过程 */
		/* 从套接字中读取数据放到缓冲区buffer中 */
		size = recv(s, buffer, 1024,0);	
		if(size == 0){/* 没有数据 */
			return;	
		}
		
		/* 构建响应字符，为接收到客户端字节的数量 */
		sprintf(buffer, "%d bytes altogether\n", size);
		send(s, buffer, strlen(buffer)+1,0);/* 发给客户端 */
	}	
}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
	ssize_t size = 0;
	char buffer[1024];	/* 数据的缓冲区 */
	
	for(;;){/* 循环处理过程 */
		/* 从标准输入中读取数据放到缓冲区buffer中 */
		size = read(0, buffer, 1024);
		if(size > 0){/* 读到数据 */
			send(s, buffer, size,0);		/* 发送给服务器 */
			size = recv(s, buffer, 1024,0);/* 从服务器读取数据 */
			write(1, buffer, size);		/* 写到标准输出 */
		}
	}	
}

void sig_proccess(int signo)
{
	printf("Catch a exit signal\n");
	_exit(0);	
}

void sig_pipe(int sign)
{
	printf("Catch a SIGPIPE signal\n");
	
	/* 释放资源 */	
}
#endif

#ifdef READVWRITEV
#include <sys/uio.h>
#include <string.h>
#include <stdlib.h>

static struct iovec *vs=NULL,*vc=NULL;
/* 服务器对客户端的处理 */
void process_conn_server(int s)
{
	char buffer[30];	/* 向量的缓冲区 */
	ssize_t size = 0;
	/* 申请3个向量 */
	struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
	if(!v){
		printf("Not enough memory\n");
		return;		
	}
	
	/**挂接全局变量，便于释放管理*/
	vs = v;
	
	/* 每个向量10个字节的空间 */
	v[0].iov_base = buffer;	/*0-9*/
	v[1].iov_base = buffer + 10;/*10-19*/
	v[2].iov_base = buffer + 20;/*20-29*/
	/*初始化长度为10*/
	v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
	
	
	
	for(;;){/* 循环处理过程 */
		/* 从套接字中读取数据放到向量缓冲区中 */
		size = readv(s, v, 3);	
		if(size == 0){/* 没有数据 */
			return;	
		}
		
		/* 构建响应字符，为接收到客户端字节的数量，分别放到三个缓冲区中 */
		sprintf(v[0].iov_base, "%d ", size); /*长度*/
		sprintf(v[1].iov_base, "bytes alt"); /*“bytes alt”字符串*/
		sprintf(v[2].iov_base, "ogether\n"); /*ogether\n”字符串*/
		/*写入字符串长度*/
		v[0].iov_len = strlen(v[0].iov_base);
		v[1].iov_len = strlen(v[1].iov_base);
		v[2].iov_len = strlen(v[2].iov_base);
		writev(s, v, 3);/* 发给客户端 */
	}	
}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
	char buffer[30];/* 向量的缓冲区 */
	ssize_t size = 0;
	/* 申请3个向量 */
	struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
	if(!v){
		printf("Not enough memory\n");
		return;		
	}
	/**挂接全局变量，便于释放管理*/
	vc = v;
	/* 每个向量10个字节的空间 */
	v[0].iov_base = buffer;/*0-9*/
	v[1].iov_base = buffer + 10;/*10-19*/
	v[2].iov_base = buffer + 20;/*20-29*/
	/*初始化长度为10*/
	v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
	
	
	
	int i = 0;
	
	for(;;){/* 循环处理过程 */
		/* 从标准输入中读取数据放到缓冲区buffer中 */
		size = read(0, v[0].iov_base, 10);
		if(size > 0){/* 读到数据 */
			v[0].iov_len= size;
			writev(s, v,1);		/* 发送给服务器 */
			v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
			size = readv(s, v, 3);/* 从服务器读取数据 */
			for(i = 0;i<3;i++){
				if(v[i].iov_len > 0){
					write(1, v[i].iov_base, v[i].iov_len);		/* 写到标准输出 */
				}
			}
		}
	}	
}

void sig_proccess(int signo)
{
	printf("Catch a exit signal\n");
	/* 释放资源 */	
	free(vc);
	free(vs);
	_exit(0);	
}

void sig_pipe(int sign)
{
	printf("Catch a SIGPIPE signal\n");
	
	/* 释放资源 */	
	free(vc);
	free(vs);
	_exit(0);
}
#endif
#ifdef RECVMSGSENDMSG
#include <sys/uio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
static struct iovec *vs=NULL,*vc=NULL;
/* 服务器对客户端的处理 */
void process_conn_server(int s)
{
	char buffer[30];	/* 向量的缓冲区 */
	ssize_t size = 0;
	struct msghdr msg;	/* 消息结构 */
	
	
	/* 申请3个向量 */
	struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
	if(!v){
		printf("Not enough memory\n");
		return;		
	}
	
	/**挂接全局变量，便于释放管理*/
	vs = v;
	
	/* 初始化消息 */
	msg.msg_name = NULL;	/*没有名字域*/
	msg.msg_namelen = 0;	/*名字域长度为0*/
	msg.msg_control = NULL;	/*没有控制域*/
	msg.msg_controllen = 0;	/*控制域长度为0*/
	msg.msg_iov = v;		/*挂接向量指针*/
	msg.msg_iovlen = 30;	/*接收缓冲区长度为30*/
	msg.msg_flags = 0;		/*无特殊操作*/
	
	/* 每个向量10个字节的空间 */
	v[0].iov_base = buffer;	/*0-9*/
	v[1].iov_base = buffer + 10;/*10-19*/
	v[2].iov_base = buffer + 20;/*20-29*/
	/*初始化长度为10*/
	v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
	
	
	
	for(;;){/* 循环处理过程 */
		/* 从套接字中读取数据放到向量缓冲区中 */
		size = recvmsg(s, &msg, 0);	
		if(size == 0){/* 没有数据 */
			return;	
		}
		
		/* 构建响应字符，为接收到客户端字节的数量，分别放到三个缓冲区中 */
		sprintf(v[0].iov_base, "%d ", size); /*长度*/
		sprintf(v[1].iov_base, "bytes alt"); /*“bytes alt”字符串*/
		sprintf(v[2].iov_base, "ogether\n"); /*ogether\n”字符串*/
		/*写入字符串长度*/
		v[0].iov_len = strlen(v[0].iov_base);
		v[1].iov_len = strlen(v[1].iov_base);
		v[2].iov_len = strlen(v[2].iov_base);
		sendmsg(s, &msg, 0);/* 发给客户端 */
	}	
}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
	char buffer[30];	/* 向量的缓冲区 */
	ssize_t size = 0;
	struct msghdr msg;	/* 消息结构 */
	
	/* 申请3个向量 */
	struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
	if(!v){
		printf("Not enough memory\n");
		return;		
	}
	
	/**挂接全局变量，便于释放管理*/
	vc = v;
	/* 初始化消息 */
	msg.msg_name = NULL;	/*没有名字域*/
	msg.msg_namelen = 0;	/*名字域长度为0*/
	msg.msg_control = NULL;	/*没有控制域*/
	msg.msg_controllen = 0;	/*控制域长度为0*/
	msg.msg_iov = v;		/*挂接向量指针*/
	msg.msg_iovlen = 30;	/*接收缓冲区长度为30*/
	msg.msg_flags = 0;		/*无特殊操作*/
	
	/* 每个向量10个字节的空间 */
	v[0].iov_base = buffer;/*0-9*/
	v[1].iov_base = buffer + 10;/*10-19*/
	v[2].iov_base = buffer + 20;/*20-29*/
	/*初始化长度为10*/
	v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
	
	
	
	int i = 0;
	
	for(;;){/* 循环处理过程 */
		/* 从标准输入中读取数据放到缓冲区buffer中 */
		size = read(0, v[0].iov_base, 10);
		if(size > 0){/* 读到数据 */
			v[0].iov_len= size;
			sendmsg(s, &msg,0);		/* 发送给服务器 */
			v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
			size = recvmsg(s, &msg,0);/* 从服务器读取数据 */
			for(i = 0;i<3;i++){
				if(v[i].iov_len > 0){
					write(1, v[i].iov_base, v[i].iov_len);		/* 写到标准输出 */
				}
			}
		}
	}	
}

void sig_proccess(int signo)
{
	printf("Catch a exit signal\n");
	/* 释放资源 */	
	free(vc);
	free(vs);
	_exit(0);	
}

void sig_pipe(int sign)
{
	printf("Catch a SIGPIPE signal\n");
	
	/* 释放资源 */	
	free(vc);
	free(vs);
	_exit(0);
}
#endif

