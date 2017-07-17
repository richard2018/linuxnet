#include <signal.h>
#include <stdio.h>

static void sig_handle(int signo)
{
	if( SIGSTOP== signo)
	{
		printf("接收到信号SIGSTOP\n");		
	}	
	else if(SIGKILL==signo)
	{
		printf("接收到信号SIGKILL\n");		
	}
	else
	{
		printf("接收到信号:%d\n",signo);		
	}
	
	return;
}

int main(void)
{
	sighandler_t ret;
	ret = signal(SIGSTOP, sig_handle);
	if(SIG_ERR == ret)
	{
		printf("为SIGSTOP挂接信号处理函数失败\n");
		return -1;		
	}	
	
	ret = signal(SIGKILL, sig_handle);
	if(SIG_ERR == ret)
	{
		printf("为SIGKILL挂接信号处理函数失败\n");
		return -1;		
	}
	
	for(;;);
	
}