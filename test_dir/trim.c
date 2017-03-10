#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

char *trim(char *s) 
{
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}

char *trim_front_and_back(char* s)               //73683
{
  int i = 0;
  int j = strlen ( s ) - 1;
  int k = 0;
 
  while ( isspace ( s[i] ) && s[i] != '\0' )
    i++;    
 
  while ( isspace ( s[j] ) && j >= 0 )
    j--;    
 
  while ( i <= j )
    s[k++] = s[i++]; 
 
  s[k] = '\0'; 
 
  return s;
}

    
