/************************************************************************************/
/*                                                                                  */
/* process_asnc.c      RUN-FILE support for "C" mains                     R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines that are used to support the OS/32 sub-system    */
/* and the RUN-FILE assignment mechanism.                                           */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   Various based upon the arguments of the functions.                             */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* goto's are used where their use would allow the code to simplified to eliminate  */
/* the deep nesting of if statements.                                               */
/*                                                                                  */
/* For in-line comments to be recognized there must be at least 1 blank or tab      */
/* preceeding the comment characters (# or *).                                      */
/*                                                                                  */
/* The "C" version of each function/subroutine is first and the FORTRAN interface   */
/* function/subroutine is second.  The FORTRAN interface function/subroutine        */
/* processes the arguments into a "C" function call.                                */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        29/Mar/1999  New file created for process_asn support    */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include Files                                                                    */
/*==================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

/*==================================================================================*/
/* Include the block data file                                                      */
/*==================================================================================*/

#include "devicecodes.h"

#include "process_asn.h"

#include "private_data.h"

#include "process_proto.h"

void Get_Lu_FD(int Lu, int * YesNo, int * UnixFd);


/*==================================================================================*/
/* "C" Interface to Process Assignment File                                         */
/*==================================================================================*/

void Process_Asn(unsigned char * BaseName, int      * status)
{

  (void) Process_Asn_Common(BaseName, status);

  return;
}

/*==================================================================================*/
/* Include the common code                                                          */ 
/*==================================================================================*/

#include "process_base.c"
#include "process_assign.c"

#include "process_runfiles.c"

#include "process_cmds.c"

#define  C_INTERFACE

#include "process_openlu.c"

/*==================================================================================*/
/* Open a File via "C" OPEN                                                         */
/*==================================================================================*/

static int      AttachLU_Language(void)
{
  int              status;
  int              fd;
  int              fd2;
  int              open_flag;
  int              protection_mode=0;

  /*--------------------------------------------------------------------------------*/
  /* Set the open mode to read only or to read/write                                */
  /*--------------------------------------------------------------------------------*/

  if(processasn.FTN_OPEN_Readonly) {
    open_flag = O_RDONLY;
  }
  else {
    open_flag = O_RDWR;
  }
    
  /*--------------------------------------------------------------------------------*/
  /* Check if renew is set and insure the file is created                           */
  /*--------------------------------------------------------------------------------*/

  if(processasn.Flag_FTN_RENEW) {
    open_flag |= O_CREAT;
  }

  /*--------------------------------------------------------------------------------*/
  /* Check if new is set and insure the file is does not currently exist            */
  /*--------------------------------------------------------------------------------*/

  if(processasn.Flag_FTN_NEW) {
    open_flag |= (O_CREAT | O_EXCL) ;
  }

  /*--------------------------------------------------------------------------------*/
  /* Now Open the file and set up the needed environment for process_common.c       */
  /*--------------------------------------------------------------------------------*/

  fd     = open((char *)processasn.Value_Filename,open_flag,protection_mode);
  if(fd >= 0L) {
    (void) Set_Lu_FD(processasn.Value_Lu,FALSE,fd);
    fd2  = open((char *)processasn.Value_Filename,open_flag,protection_mode);
    if(fd2 >= 0L) {
      (void) Set_Lu_FD(processasn.Value_Lu2,FALSE,fd2);
    }
    status  = 0L;
  }
  else {
    status = errno;
  }

  /*--------------------------------------------------------------------------------*/
  /* Return to the caller                                                           */
  /*--------------------------------------------------------------------------------*/

  return(status);
}


/*==================================================================================*/
/* Dettach a file via "C" close                                                     */
/*==================================================================================*/

static int      DettachLU_Language(int       lu)
{
  int             status;
  int             fd;
  int             istcp;

  (void) Get_Lu_FD(lu,&istcp,&fd);

  if(fd >= 0L) {
    status = close(fd);
    if(status < 0L) {
      status = errno;
    }
    else {
      status = 0L;
    }
  }
  else {
    status = 0L;
  }
  return(status);
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
