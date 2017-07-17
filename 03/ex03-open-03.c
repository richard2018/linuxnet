/*文件ex03-open-03.c,
O_CREAT和O_EXCL的使用*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
  int fd = -1;
  char filename[] = "test.txt";
  /*打开文件，如果文件不存在，则报错*/
	fd = open(filename,O_RDWR|O_CREAT|O_EXCL,S_IRWXU);
	if(-1 == fd){
		/*文件已经存在*/
	  printf("File %s exist!,reopen it",filename);
	  /*重新打开*/
	  fd = open(filename,O_RDWR);
	  printf("fd:%d\n",fd);
	}	else	{
		/*文件不存在，创建并打开*/
	  printf("Open file %s success,fd:%d\n",filename,fd);
	}

	return 0;
}
