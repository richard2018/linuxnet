
#define ENDSTRING '\0'
int StrLen(char *string)
{
  int len = 0;
  while(*string++ != ENDSTRING)
    len++;
  return len;
}

