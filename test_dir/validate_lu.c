#include <stdio.h>
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

int validate_lu_write_(int *lu)
{
   FILE *fp = fdopen(getfd_(lu), "w");
   if (fp == (FILE *)NULL)
   {
       return 0;
   }
   else
   {
       return 1; 
   }
}
int validate_lu_read_(int *lu)
{
   FILE *fp = fdopen(getfd_(lu), "r");
   if (fp == (FILE *)NULL)
   {
       return 0;
   }
   else
   {
       return 1; 
   }
}
int validate_lu_readwrite_(int *lu)
{
   FILE *fp = fdopen(getfd_(lu), "w+");
   if (fp == (FILE *)NULL)
   {
       return 0;
   }
   else
   {
       return 1; 
   }
}
