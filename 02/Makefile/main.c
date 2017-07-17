/* main.c */
#include <stdio.h>
#include "add.h"
#include "sub.h"

int main(void)
{
	int input = 0;
	int a = 10, b = 12;
	float x= 1.23456,y = 9.87654321;
	
	printf("int a+b IS:%d\n",a+b);
	printf("int a-b IS:%d\n",a-b);
	printf("float x+y IS:%f\n",x+y);
	printf("float x-y IS:%f\n",x-y);
	
	return 0;	
}

