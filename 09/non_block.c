
#define PORT 9999
#define BACKLOG 4
int main(int argc, char *argv[]) 
{ 
	streuct sockaddr_in local,client;
	int len;
	int s_s  = -1,s_c= -1;
	/* 初始化地址结构 */
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr = htonl(-1);
	
	/*建立套接字描述符*/
	s_s = socket(AF_INET, SOCK_STREAM, 0);
	/*设置非阻塞方式*/
	fcntl(s_s,F_SETFL, O_NONBLOCK); 
	/*侦听*/
	listen(s_s, BACKLOG);
	for(;;)
	{
		/*轮询接收客户端*/
		while(s_c < 0){/*等待客户端到来*/
			s_c =accept(s_s, (struct sockaddr*)&client, &len);
		}
		
		/*轮询接收，当接收到数据的时候退出while循环*/
		while(recv(s_c, buffer, 1024)<=0)
			;
		/* 接收到客户端的数据 */
		if(strcmp(buffer, "HELLO")==0){/*判断是否为HELLO字符串*/
			send(s, "OK", 3, 0);/*发送响应*/
			close(s_c);			/*关闭连接*/
			continue;			/*继续等待客户端连接*/
		}
		
		if(strcmp(buffer, "SHUTDOWN")==0){/*判断是否为SHUTDOWN字符串*/
			send(s, "BYE", 3, 0);	/*发送BYE字符串*/
			close(s_c);				/*关闭客户端连接*/
			break;					/*退出主循环*/
		}
		
	}
	close(s_s);
	
	return 0;
}