/*文件ex03-fcntl-02.c,
使用fcntl修改文件的状态值*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>/*strlen函数*/

int main(void)
{
	int flags = -1;
	char buf[] = "FCNTL";
	
	int fd = open("test.txt", O_RDWR);
	/*获得文件状态*/	
	flags = fcntl(fd, F_GETFL, 0);
	/*增加状态为可追加*/
	flags |= O_APPEND;
	/*将状态写入*/
	flags = fcntl(fd, F_SETFL, &flags);
  if( flags < 0 ){
  	/*错误发生*/
  	printf("failure to use fcntl\n");
  	return -1;
  }
  /*向文件中写入字符串*/
  write(fd, buf, strlen(buf));
  close(fd);
 	
	return 0;
}

