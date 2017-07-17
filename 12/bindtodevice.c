

int s,err;
char ifname[] = "eth1";	/* 绑定的网卡名称 */

/* 一些处理过程 */
...
...

err = setsockopt(s, SOL_SOCKET, SO_BANDTODEVICE, ifname, 5);	/* 将s绑定到网卡eth1上 */
if(err){/* 失败 */
	printf("setsockopt SO_BANDTODEVICE failure\n");	
}
