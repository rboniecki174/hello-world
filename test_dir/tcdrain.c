/************************************************************************************/
/*                                                                                  */
/* tcdrain.c    Invoke the tcdrain() function from FORTRAN                R00-00.01 */
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
/* R00-00.01  DRLJr    07/Apr/1999  Changed references of getfd_ to GetFD_ so       */
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
// tt80093   gj        22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <termios.h>
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

void tcdrain_(int      * lu)
{
  int         status;
  int         IsTcp;
  int         UnixFd;

  (void) get_lu_fd_(lu,&IsTcp,&UnixFd);
  if(UnixFd < 0L) {
    UnixFd = GetFD_(lu);
  }

  status = tcdrain(UnixFd);

  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
