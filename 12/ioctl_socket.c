#include 

int main(void)
{
	int s  =-1;		/* socket描述符 */
	int err = -1;	/* 返回值 */
	
	......
	......
	
	
	int request = -1;/* 请求类型 */
	int para = -1;	/* ioctl第三个参数 */
	struct timeval tv;/* ioctl第三个参数 */
	
	/* 有无带外数据判断 */	
	request = SIOCATMARK;
	err = ioctl(s, request, &para);
	if(err){/* ioctl函数出错 */
		/* 错误处理 */	
	}
	if(para){ /* 有带外数据 */
		/* 接收带外数据，处理... */
		...
	}else{/*无带外数据*/
		...
	}
	
	
	
	/* 获得SIGIO和SIGURG信号处理进程ID */
	request = SIOCGPGRP;/*或者FIOGETOWN*/
	err = ioctl(s, request, &para);
	if(err){/* ioctl函数出错 */
		/* 错误处理 */
		...
	}else{
		/* 获得了处理信号的进程ID号 */
		...
	}
	
	
	
	/* 设置SIGIO和SIGURG信号处理进程ID */
	request = SIOCSPGRP;/*FIOSETOWN*/
	err = ioctl(s, request, &para);
	if(err){/* ioctl函数出错 */
		/* 错误处理 */
		...
	}else{
		/* 成功设置了处理信号的进程ID号 */
		...
	}
	
	
	
	/* 获得数据报文到达的时间 */	
	request = SIOCGSTAMP;/*FIOSETOWN*/
	err = ioctl(s, request, &tv);
	if(err){/* ioctl函数出错 */
		/* 错误处理 */
		...	
	}else{
		/* 获得了最后数据报文到达时间，在参数tv内 */
		...
	}
	
	...
	...	
}