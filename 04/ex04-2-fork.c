#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
	pid_t pid;
	
	/* 分叉进程 */
	pid = fork();
	
	/* 判断是否执行成功 */
	if(-1 == pid){
		printf("进程创建失败！\n");
		return -1;
	} else if(pid == 0){
		/* 子进程中执行此段代码 */
		printf("子进程，fork返回值：%d, ID:%d, 父进程ID:%d\n",pid, getpid(), getppid());
	}	else{
		/* 父进程中执行此段代码 */
		printf("父进程，fork返回值：%d, ID:%d, 父进程ID:%d\n",pid, getpid(), getppid());
	}
	
	return 0;
}

