/*==================================================================================*/
/* FORTRAN GetFD_() code - required to support I/O subsystem                        */
/*==================================================================================*/
#include <stdio.h>
#include "private_data.h"

//2014-02-21    jeffs  78919 Changes to libos32 for F to C conversion.
//                     prepare to Remove reference to FNUM in getfd.

 
int getfd(int lu)
{
   int fd;
   if((lu < 0) || (lu >= MAXIMUM_LU)) {
     fprintf(stderr,">>getfd: Error in range.  LU %d\n",lu);
     return( -1 );   //error 
   } 
//old way this was done in fortran was using intrinisc getfnum:
   fd = fnum_store[lu];
   return fd;
}

int getfd_(int *lu)
{
   return getfd(*lu);
}

int      GetFD_(const int      * lu)
{
   int             fd;
 
   fd    = getfd_((int *)lu);
   if(fd < 0L) {
     fd    = private_data.lu_to_fd[*lu];
   }
   return(fd);
 
}
