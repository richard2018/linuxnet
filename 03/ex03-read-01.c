/*文件ex03-open-03.c,
O_CREAT和O_EXCL的使用*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
  int fd = -1,i;
  ssize_t size = -1;
  /*存放数据的缓冲区*/
  char buf[10];
  char filename[] = "test.txt";
  /*打开文件，如果文件不存在，则报错*/
	fd = open(filename,O_RDONLY);
	if(-1 == fd){
		/*文件已经存在*/
	  printf("Open file %s failure,fd:%d\n",filename,fd);
	}	else	{
		/*文件不存在，创建并打开*/
	  printf("Open file %s success,fd:%d\n",filename,fd);
	}

	/*循环读取数据，直到文件末尾或者出错*/
	while(size){
		/*每次读取10个字节数据*/
		size = read(fd, buf,10);
		if( -1 == size)	{
			/*读取数据出错*/
			close(fd);/*关闭文件*/
			printf("read file error occurs\n");
			/*返回*/
			return -1;		
		}else{
			/*读取数据成功*/
			if(size >0 ){
				/*获得size个字节数据*/
				printf("read %d bytes:",size);
				/*打印引号*/
				printf("\"");
				/*将读取的数据打印出来*/
				for(i = 0;i<size;i++){
					printf("%c",*(buf+i));
				}
				/*打印引号并换行*/
				printf("\"\n");
			}else{
				printf("reach the end of file\n");	
			}
		}
	}

	return 0;
}
