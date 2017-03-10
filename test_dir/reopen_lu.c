/************************************************************************************/
/*                                                                                  */
/* reopen_lu.c  Re-open Lu on a file or device                            R00-01.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who      When         Why                                             */
/* =========  =======  ===========  =============================================== */
/* R00-00.00  DRLJr    03/Mar/1999  Created File                                    */
/* R00-00.01  DRLJr    07/Apr/1999  Added a better header                           */
/* R00-01.00  DRLJr    07/Apr/1999  Changed references of getfd_ to GetFD_ so       */
/*                                  code can be used in both "C" and FORTRAN.       */
/*                                  GetFD_() is located in process_asnc.c and       */
/*                                  process_asnf.c.  The file process_asnc.c        */
/*                                  is used for "C" main programs and the file      */
/*                                  process_asnf.c is used for FORTRAN main         */
/*                                  programs.  For FORTRAN main programs the        */
/*                                  routine GetFD_() calls the FORTRAN RTL          */
/*                                  routine getfd_() and checks the shared          */
/*                                  memory segments reatime1 if getfd_()            */
/*                                  returns a -1 value.  For the "C" main           */
/*                                  programs the shared memory segment is used      */
/*                                  to detimine the unix file descriptor based      */
/*                                  on the supplied logical unit.                   */
// tt80093    gj       22/May/2014  Clean up ptorotypes
/*                                                                                  */
/************************************************************************************/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dlogdefs.h>
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define  MODE (O_RDWR | O_APPEND)

static char fmt[] =
   {"reopen_lu_ failure (%1.1s) : errno = %d (%s), File=<%s>" };

void reopen_lu_(int      * lu, char * file, int      * opnstat, 
                int        file_len)
{
  int          fd1;                /* Fd lu is assigned    */
  int          fd2;                /* A second fd for work */
  int          mvlen;              /* Length of move data  */
  int          index;              /* Working index        */
  int          status;             /* dup2 status          */
  char         cfile[256];         /* Work copy of the file*/
  char       * errstr;             /* Error string pointer */

  mvlen     =  file_len;           /* Length of the buffer */
  if(mvlen > 256-1) {              /* Leave room for null  */
    mvlen = 256-1;                 /*                      */
  }                                /*                      */

  memset(cfile,0x00,256);          /* Zero the buffer      */

  /* Move the file descriptor and make into a "C" string   */
  for(index=0;index<mvlen;index++) {
    if((file[index] <= ' ') || (file[index]>'~')) {
      break;
    }
    cfile[index] = file[index];
  }

  fd1  = GetFD_(lu);               /* Get the FORTRAN lu   */
  fd2  = open(cfile,MODE);         /* Open the file        */
/*dLog(DLOG_MINOR,"FORTRAN LUs are %3d and %3d\n",fd1,fd2);*/
  if(fd2 < 0L) {                   /* Error                */
    errstr = strerror(errno);      /* Get the error code   */
    dLog(DLOG_MAJOR,fmt,'0',errno,errstr,cfile);
    (*opnstat) = -1L;
    return;
  }

  errno  = 0L;                     /* Clear the status     */
  status = dup2(fd2,fd1);          /* Move lu fd1 to fd2   */
  if(status < 0L) {                /* The open failed      */
    errstr = strerror(errno);      /* Get the error code   */
    dLog(DLOG_MAJOR,fmt,"1",errno,errstr,cfile);
    (*opnstat) = -2L;
    return;
  }                                /*                      */
  status = close(fd2);             /* Close the work fd2   */

  return;
}

/***********************************************************/
/*                                                         */
/***********************************************************/
