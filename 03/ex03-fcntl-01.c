/*文件ex03-fcntl-01.c,
使用fcntl控制文件符*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
	int flags = -1;
	int accmode = -1;
	
	/*获得标准输入的状态的状态*/
	flags = fcntl(0, F_GETFL, 0);
  if( flags < 0 ){
  	/*错误发生*/
  	printf("failure to use fcntl\n");
  	return -1;
  }
  
  /*获得访问模式*/
  accmode = flags & O_ACCMODE;  
  if(accmode == O_RDONLY)/*只读*/
  	printf("STDIN READ ONLY\n");
  else if(accmode == O_WRONLY)/*只写*/
  	printf("STDIN WRITE ONLY\n");
  else if(accmode ==O_RDWR)/*可读写*/
  	printf("STDIN READ WRITE\n");
  else/*其他模式*/
  	printf("STDIN UNKNOWN MODE");
  	
  if( flags & O_APPEND )
  	printf("STDIN APPEND\n");
  if( flags & O_NONBLOCK )
  	printf("STDIN NONBLOCK\n");
 	
	return 0;
}

