/*文件ex03-fcntl-02.c,
使用fcntl修改文件的状态值*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>/*strlen函数*/

#define NEWFD 8
int main(void)
{
	char buf[] = "FCNTL";
	
	int fd = open("test.txt", O_RDWR);
	printf("the file test.txt ID is %d\n",fd);
	/*获得文件状态*/	
	fd = fcntl(fd, F_GETFD);
	printf("the file test.txt ID is %d\n",fd);
	fd = NEWFD;
	/*将状态写入*/
	fcntl(NEWFD, F_SETFL, &fd);
  
  /*向文件中写入字符串*/
  write(NEWFD, buf, strlen(buf));
  close(NEWFD);
 	
	return 0;
}

