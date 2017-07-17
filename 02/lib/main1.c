#include "ex02.h"
#include <dlfcn.h>
int main(void)
{
  char src[]="Hello Dymatic";
  char desc[80];

#if 0  
  printf("string length is:%d\n",StrLen(src));
  printf("string desc is:%s\n",StrCopy(desc,src));
#else
  int (*pStrLenFun)(char *str);
  char *(*pStrCopyFun)(char *desc, char *src);

  void *phandle = NULL;
  char *perr = NULL;
  phandle = dlopen("./libstr.so", RTLD_LAZY);
  if(!phandle)
  {
    printf("Failed Load library!\n");
  }
  perr = dlerror();
  if(perr != NULL)
  {
    printf("%s\n",perr);
    return 0;
  }
  
  pStrLenFun = dlsym(phandle, "StrLen");
  perr = dlerror();
  if(perr != NULL)
  {
    printf("%s\n",perr);
    return 0;
  }

  pStrCopyFun = dlsym(phandle, "StrCopy");
  perr = dlerror();
  if(perr != NULL)
  {
    printf("%s\n",perr);
    return 0;
  }


  printf("The string is: %s\n",src);
  printf("the string length is: %d\n",pStrLenFun(src));
  printf("the string copyed:%s\n",pStrCopyFun(desc,src));
#endif
  return 0;
}
