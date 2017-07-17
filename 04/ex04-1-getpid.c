#include <sys/types.h>
#include <unistd.h>
#include<stdio.h>

int main()
{
	pid_t pid,ppid;
	
	/* 获得当前进程和其父进程的ID号 */
	pid = getpid();
	ppid = getppid();
	
	printf("当前进程的ID号为：%d\n",pid);
	printf("当前进程的的父进程号ID号为：%d\n",ppid);
	
	return 0;
}
