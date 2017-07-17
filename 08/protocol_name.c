#include <netdb.h>
#include <stdio.h>
 
 /* 显示协议的项目 */
void display_protocol(struct protoent *pt)
{
	int i = 0;
	if(pt){/*合法的指针*/
		printf("protocol name:%s,",pt->p_name);/*协议的官方名称*/
		if(pt->p_aliases){/*别名不为空*/
			printf("alias name:");/*显示别名*/
			while(pt->p_aliases[i]){/*列表没到结尾*/
				printf("%s ",pt->p_aliases[i]);	/*显示当前别名*/
				i++;/*下一个别名*/
			}
		}
		printf(",value:%d\n",pt->p_proto);	/*协议值*/
	}
}

int main(int argc, char *argv[])
{
	int i = 0;
	/* 要查询的协议名称 */
	const char  *const protocol_name[]={
 	"ip",
 	"icmp",
 	"igmp",	
 	"ggp",
 	"ipencap",
 	"st",
 	"tcp",
 	"egp",
 	"igp",
 	"pup",
 	"udp",
 	"hmp",
 	"xns-idp",
 	"rdp",
 	"iso-tp4",
 	"xtp",
 	"ddp",
 	"idpr-cmtp",
 	"ipv6",
 	"ipv6-route",
 	"ipv6-frag",
 	"idrp",
 	"rsvp",
 	"gre",
 	"esp",
 	"ah",
 	"skip",
 	"ipv6-icmp",
 	"ipv6-nonxt",
 	"ipv6-opts",
 	"rspf",
 	"vmtp",
 	"eigrp",
 	"ospf",
 	"ax.25",
 	"ipip",
 	"etherip",
 	"encap",
 	"pim",
 	"ipcomp",
 	"vrrp",
 	"l2tp",
 	"isis",
 	"sctp",
 	"fc",
 	NULL}; 
	
	setprotoent(1);/*在使用函数getprotobyname时不关闭文件/etc/protocols*/
	while(protocol_name[i]!=NULL){/*没有到数组protocol_name的结尾*/
		struct protoent *pt = getprotobyname((const char*)&protocol_name[i][0]);/*查询协议*/
		if(pt){/*成功*/
			display_protocol(pt);	/*显示协议项目*/
		}
		i++;/*移到数组protocol_name的下一个*/
	};
	endprotoent();/*关闭文件/etc/protocols*/
	return 0;	
}
