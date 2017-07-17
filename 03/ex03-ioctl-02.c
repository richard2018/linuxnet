/*文件ex03-ioctl-01.c
*控制CDROM*/
#include <linux/cdrom.h>
#include <stdio.h>
#include <fcntl.h>

int main(void){
	/*打开CDROM设备文件*/
	int fd = open("/dev/cdrom",O_RDONLY);
	if(fd < 0){
		printf("打开CDROM失败\n");
		return -1;
	}
	if (!ioctl(fd, CDROMEJECT,NULL)){
		printf("成功弹出CDROM\n");
	}else{
		printf("弹出CDROM失败\n");
	}
	return 0;
}
