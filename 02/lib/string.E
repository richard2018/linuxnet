# 1 "string.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "string.c"


int StrLen(char *string)
{
  int len = 0;
  while(*string++ != '\0')
    len++;
  return len;
}
