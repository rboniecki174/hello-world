// readnum_lu -- get a single valid number from tty or file input. 
//
// 2012-2-19  jeffs      78919 Convert file opens to C from fortran.
// 2014-6-4   gj         tt80093 Clean up prototypes

//#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "string.h"
#include "strings.h"
#include "proto_common.h"
#include "dlogdefs.h"

//int readline(int lu, char *buff, int maxlen); tt80093

int readnum_lu(int lu, int *num)
{
   int istat; 
   int status;
   char numstr[32];
   memset(numstr,0,32);
   istat = readline(lu, numstr, sizeof(numstr));
   //fprintf(stderr,"got a string <%s> on lu %d isstat-%d\n",numstr,lu,istat);
   if (istat < 0) {
      //got an error. bail with -1
      // note that *num is undefined
      return(-1);
   }   
   //fprintf(stderr,"got a string <%s>\n",numstr);
   //now see if we can get a number from it.
   status = sscanf(numstr,"%d", num);                       /* Get the number         */

   if(status < 1L) {                                      /* If nothing converted   */
     //fprintf(stderr,"sscanf error, status = %d\n",status);
     *num = 0;
     return(-1L);                                         /* Report error           */
   }
   else {
     //fprintf(stderr,"got a number %d\n",*num);
     return(0); 
   }
}

int readnum_lu_(int *lu, int *num)
{
    return readnum_lu(*lu, num);
}

int readnum_lu5_(int *num)
{
    return readnum_lu( 5, num);
}
