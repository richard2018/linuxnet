#include <stdio.h>
//#include <math.h>
extern int StrLen(char* str);
int main(void)
{
  char src[]="Hello Dymatic";
  float f = sin(0);
  printf("the value of sin(0) is:%f\n",f);
  printf("string length is:%d\n",StrLen(src));
  return 0;
}
