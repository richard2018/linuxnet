 #include <netdb.h>
 #include <string.h>
 #include <stdio.h>
 #include <sys/socket.h> 
 #if 0
 struct hostent {
                      char    *h_name;        /* official name of host */
                      char    **h_aliases;    /* alias list */
                      int     h_addrtype;     /* host address type */
                      int     h_length;       /* length of address */
                      char    **h_addr_list;  /* list of addresses */
              }
              #define h_addr  h_addr_list[0]  /* for backward compatibility */
struct hostent *
       gethostbyaddr(const void *addr, int len, int type);
#endif
 int main(int argc, char *argv[])
 {
 	struct hostent *ht=NULL;
 
 	/* 查询的主机域名 */
 	char host[]="www.sina.com.cn";
 	
 #if 1
 	struct hostent *ht1=NULL, *ht2=NULL;
 	char host1[]="www.sohu.com";
 	/* 查询主机www.sina.com.cn */
 	ht1 = gethostbyname(host);
 	ht2 = gethostbyname(host1);
 	int j = 0;
 	
 #else
 	struct in_addr in;
 	in.s_addr = inet_addr("60.215.128.140");
 	ht = gethostbyaddr(&in, sizeof(in), AF_INET);
 #endif
 for(j = 0;j<2;j++){
 	if(j == 0)
 		ht = ht1;
 	else
 		ht =ht2;
 		
 	printf("----------------------\n");
 	
 	if(ht){
 		int i = 0;
 		printf("get the host:%s addr\n",host);	/* 原始域名 */
 		printf("name:%s\n",ht->h_name);			/* 名称 */
 		
 		/*协议族AF_INET为IPv4或者AF_INET6为IPv6*/
 		printf("type:%s\n",ht->h_addrtype==AF_INET?"AF_INET":"AF_INET6");
 		
 		/* IP地址的长度 */
 		printf("legnth:%d\n",ht->h_length);	
 		/* 打印IP地址 */
 		for(i=0;;i++){
 			if(ht->h_addr_list[i] != NULL){/* 不是IP地址数组的结尾 */
 				printf("IP:%s\n",inet_ntoa((unsigned int*)ht->h_addr_list[i]));	/*打印IP地址*/
 			}	else{/*达到结尾*/
 				break;	/*退出for循环*/
 			}
 		}
 		
 		/* 打印域名地址 */
 		for(i=0;;i++){/*循环*/
 			if(ht->h_aliases[i] != NULL){/* 没有到达域名数组的结尾 */
 				printf("alias %d:%s\n",i,ht->h_aliases[i]);	/* 打印域名 */
 			}	else{/*结尾*/
 				break;	/*退出循环*/
 			}
 		}
 	}	
}
 	return 0;
 }
 