/*文件名：ex02-gdb-01.c*/
#include <stdio.h>/* 用于printf*/
#include <stdlib.h>/*用于malloc*/

/* 声明函数sum为static int类型 */
static int sum(int value);

/* 用于控制输入输出的结构 */
struct inout{
		int value;
		int result;
};
int main(int argc, char *argv[]){
	/*申请内存*/
	struct inout *io = (struct inout*)malloc(sizeof(struct inout));
	/*判断是否成功*/
	if(NULL == io)
	{
		/*失败返回*/
		printf("申请内存失败\n");
		return -1;		
	}
	
	/*判断输入参数是否正确*/
	if(argc !=2)
	{
		printf("参数输入错误!\n");
		return -1;		
	}
	
  /* 获得输入的参数 */
  io->value = *argv[1]-'0';
  /* 对value进行累加求和 */
  io->result = sum(io->value);
  printf("你输入的值为：%d,计算结果为：%d\n",io->value,io->result);

  return 0;
}

/* 累加求和函数 */
static int sum(int value){
  int result = 0;
  int i = 0;
  /* 循环计算累加值 */
  for(i=0;i<value;i++)
    result += (i+1);
  /*返回结果*/
  return result;
}
