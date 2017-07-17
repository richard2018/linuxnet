/*文件ex03-lseek-02.c,
使用lseek函数构建空洞文件*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
  int fd = -1,i;
  ssize_t size = -1;
  off_t offset = -1;  
  /*存放数据的缓冲区*/
  char buf1[]="01234567";
  char buf2[]="ABCDEFGH";
  /*文件名*/
  char filename[] = "hole.txt";  
  int len = 8;
  
  /*创建文件hole.txt*/
	fd = open(filename,O_RDWR|O_CREAT,S_IRWXU);
	if(-1 == fd){
		/*创建文件失败*/
	  return -1;
	}	

	/*将buf1中的数据写入到文件Hole.txt中*/
	size = write(fd, buf1,len);
	if(size != len){
		/*写入数据失败*/
		return -1;	
	}
	
	/*设置文件偏移量为绝对值的32*/
	offset = lseek(fd, 32, SEEK_SET);
	if(-1 == offset){
		/*设置失败*/
		return -1;
	}
	/*将buf2中的数据写入到文件hole.txt中*/
	size = write(fd, buf2,len);
	if(size != len){
		/*写入数据失败*/
		return -1;	
	}
		
	/*关闭文件*/
	close(fd);

	return 0;
}
