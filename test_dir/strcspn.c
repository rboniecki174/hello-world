#include <stdio.h>
#include <string.h>
#include <strings.h>
int strcspn_(char *str)
{
   int len = strcspn(str," ");
   if (len > 0) 
      return(len);
   else
      return (strlen(str));
}
