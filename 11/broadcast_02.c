01		#define CMD_DISCOVERY "CMD_DISCOVERY"        /*IP发现命令*/
02		#define CMD_DISCOVERY_ACK "CMD_DISCOVERY_ACK"/*IP发现应答命令*/
03		#define IFNAME "eth0"
04		void	IPFound(void *arg)
05		{
06		#define BUFFER_LEN 32
07			int ret = -1;
08			SOCKET sock = -1;
09			int so_broadcast = 1;
10			struct ifreq ifr;  			
11			struct sockaddr_in broadcast_addr;	/*本地地址*/
12			struct sockaddr_in from_addr;	/*服务器端端地址*/
13	    	int from_len;
14			int count = -1;
15			fd_set readfd;
16			char buff[BUFFER_LEN];
17			struct timeval timeout;	
18			timeout.tv_sec = 2;	/*超时时间2秒*/
19			timeout.tv_usec = 0;
20		
21			
22	    	sock = socket(AF_INET, SOCK_DGRAM, 0);/*建立数据报套接字*/
23			if( sock < 0 )
24			{
25				DBGPRINT("HandleIPFound: socket init error\n");
26				return;
27			}
28			/*拷贝需要使用的网络接口*/
29			strcpy(ifr.ifr_name,IFNAME,strlen(IFNAME));
30			/*发送命令，获取网络接口的广播地址*/
31			if(ioctl(sock,SIOCGIFBRDADDR,&ifr) == -1)
32				perror("ioctl error"),exit(1);
33			/*将获得的广播地址拷贝给变量broadcast_addr*/
34			memcpy(&broadcast_addr, &ifr.ifr_broadaddr, sizeof(struct sockaddr_in ));
35			broadcast_addr.sin_port = htons(MCAST_PORT);/*设置广播端口*/
36			
37			/*设置套接字文件描述符sock为可以进行广播操作*/
38			ret = setsockopt(sock,
39			        SOL_SOCKET,
40			        SO_BROADCAST,
41		        &so_broadcast,
42		        sizeof so_broadcast);
43		        
44	    	/*主处理过程*/
45			int times = 10;
46			int i = 0;
47			for(i=0;i<times;i++)
48			{
49				/*广播发送服务器地址请求*/
50				ret = sendto(sock,
51							CMD_DISCOVERY,
52							strlen(CMD_DISCOVERY),
53							0,
54							(struct sockaddr *)&broadcast_addr,
55							sizeof(broadcast_addr));
56				if(ret == -1){
57					continue;	
58				}
59				/*文件描述符集合清零*/
60				FD_ZERO(&readfd);
61				/*将套接字文件描述符加入读集合*/
62				FD_SET(sock, &readfd);
63				/*select侦听是否有数据到来*/
64				ret = selectsocket(sock+1, &readfd, NULL, NULL, &timeout);
65				switch(ret)
66	    		{
67					case -1:
68						/*发生错误*/
69						break;
70					case 0:
71						/*超时*/
72						//DO timeout THING
73						
74						break;
75					default:
76	    				/*有数据到来*/
77						if( FD_ISSET( sock, &readfd ) )
78						{
79							/*接收数据*/
80							count = recvfrom( sock, buff, BUFFER_LEN, 0, ( struct sockaddr * ) &from_addr, &from_len );
81							DBGPRINT( "Recv msg is %s\n", buff );
82							if( strstr( buff, CMD_DISCOVERY_ACK ) )/*判断是否吻合*/
83							{
84								printf("found server, IP is %s\n",inet_ntoa(from_addr.sin_addr));
85							}
86						break;/*成功获得服务器地址，退出*/
07						}
08				}
09			}	
90			return;
91		}