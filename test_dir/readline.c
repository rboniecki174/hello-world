// readline -- a replacement for fortran READ
//
// 2012-2-19  jeffs      78919 Convert file opens to C from fortran.
//                       New routine

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "proto_common.h"
#include "errno.h"
#include "string.h"
#include "dlogdefs.h"

int readline(int lu, char *buff, int maxlen)
{
   int istat, fd; 
   int len = 0;
   int pos = 0;
   char c;
   char locbuf[1024];
   

   fd = getfd_(&lu);
   memset(locbuf,0,sizeof locbuf);
   while ( (len=read(fd, &c, 1)) > 0 ){
      if (c == '\n') {
        //fprintf(stderr,"CR with pos=%d\n", pos);
          break;
      }
      else {
        //fprintf(stderr,"got c=%d pos=%d\n",c,pos);
          locbuf[pos++] = c;
      }
   }
//-------------------------------------------------------
//now make the return value emulate the fortran read:
// fortran read gets the following status returns:
//    0 => success
//   -1 => EOF
//   positive number => error
//-------------------------------------------------------

   if (len == 0) {       //EOF
     istat = -1;
   }
   else if (len > 0) {   //OK
     memcpy(buff,locbuf,maxlen);
     //fprintf(stderr,"OK return buff = %.*s\n", maxlen, buff);
     istat = 0;
   }
   else {                //error is negative return
     //convert negative to errno return
     dLog(DLOG_MAJOR,"For LU %d ERRNO=%d (%s)", lu, errno, strerror(errno)); 
     istat = errno;
   }
   return istat;
}

int readline_(int *lu, char *buff, int *maxlen)
{
    return readline(*lu, buff, *maxlen);
}
