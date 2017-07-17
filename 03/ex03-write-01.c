/*文件ex03-write-01.c,
O_CREAT和O_EXCL的使用*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
  int fd = -1,i;
  ssize_t size = -1;
  int input = 0;
  /*存放数据的缓冲区*/
  char buf[]="quick brown fox jumps over the lazy dog";
  char filename[] = "test.txt";
  /*打开文件，如果文件不存在，则报错*/
	fd = open(filename,O_RDWR|O_TRUNC);
	if(-1 == fd){
		/*文件已经存在*/
	  printf("Open file %s failure,fd:%d\n",filename,fd);
	}	else	{
		/*文件不存在，创建并打开*/
	  printf("Open file %s success,fd:%d\n",filename,fd);
	}

	/*将数据写入到文件test.txt中*/
	size = write(fd, buf,strlen(buf));
	printf("write %d bytes to file %s\n",size,filename);
	/*关闭文件*/
	close(fd);

	return 0;
}
