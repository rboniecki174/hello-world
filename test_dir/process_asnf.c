/************************************************************************************/
/*                                                                                  */
/* process_asnf.c      RUN-FILE support for FORTRAN mains                 R00-00.01 */
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
/* R00-00.01  DRLJr        24/May/1999  Added the routine get_firsttaskargnumb_()   */
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

int lnblnk(unsigned char * buffer, int        length);
int lnblnk_(unsigned char * buffer, int        length);
void  getarg_(int      * argno, unsigned char *arg, int      arglen);

/*==================================================================================*/
/* Include the block data file                                                      */
/*==================================================================================*/

#include "devicecodes.h"

#include "process_asn.h"

#include "private_data.h"

#include "process_proto.h"

/*==================================================================================*/
/* Various Macros                                                                   */
/*==================================================================================*/


/*==================================================================================*/
/* FORTRAN Callable Version of Process_Asn()                                        */
/*==================================================================================*/

void process_asn_c(char *BaseName, int      * status)                         /*stoa*/
{
  int             length;

  length              = lnblnk((unsigned char *)BaseName,WRK_NAMESIZE);
  BaseName[length+1L] = '\0';

//fprintf(stderr,"Calling Process_Asn_Common(%s,status)\n",BaseName);

  (void) Process_Asn_Common((unsigned char *)BaseName,status);

//fprintf(stderr,"Return  Process_Asn_Common(%s,%6d)\n",BaseName,(*status));

  return;
}
void process_asn_(int      * status)
{
  int             decimal_one;
  int             length;
  UCHAR           BaseName[WRK_NAMESIZE];

  decimal_one   = 1L;
  memset(BaseName, (int     ) ' ', WRK_NAMESIZE);

  (void) getarg_(&decimal_one, (unsigned char *)BaseName, WRK_NAMESIZE-2L);

  length              = lnblnk(BaseName,WRK_NAMESIZE);
  BaseName[length+1L] = '\0';

  IF_DEBUG(1)
    fprintf(stderr,"Calling Process_Asn_Common(%s,status)\n",BaseName);
  END_DEBUG

  (void) Process_Asn_Common(BaseName,status);

  IF_DEBUG(1)
    fprintf(stderr,"Return  Process_Asn_Common(%s,%6d)\n",BaseName,(*status));
  END_DEBUG

  return;
}

/*==================================================================================*/
/* Include the common code                                                          */
/*==================================================================================*/

#include "process_base.c"
#include "process_assign.c"


#include "process_runfiles.c"

#include "process_cmds.c"

#define  FTN_INTERFACE

#include "process_openlu.c"

void printbufr(unsigned char * title, unsigned char * buffer, int      length);
int lnblnk(unsigned char * buffer, int        length);
void process_ftnopen_(int *lu,
                      unsigned char *File,
                      unsigned char *Access,
                      unsigned char *Status,
                      unsigned char *form,
                      int  *reclen,
                      int  *readonly,
                      int  *retstatus,
                      int str1len, int str2len, int str3len, int str4len);

void process_ftnclose_(int *lu, int *retstat);
                      
/*==================================================================================*/
/* Open a File via FORTRAN OPEN                                                     */
/*==================================================================================*/

static int      AttachLU_Language(void)
{
  int              status;
  int              status2;

  (void) process_ftnopen_(&processasn.Value_Lu,
                           processasn.Value_Filename,
                           processasn.FTN_OPEN_Access,
                           processasn.FTN_OPEN_Status,
                           processasn.FTN_OPEN_Form,
                          &processasn.FTN_OPEN_Recl,
                          &processasn.FTN_OPEN_Readonly,
                          &status,
                           strlen((char *)processasn.Value_Filename),
                           lnblnk_((unsigned char*)processasn.FTN_OPEN_Access,12L),
                           lnblnk_((unsigned char*)processasn.FTN_OPEN_Status,12L),
                           lnblnk_((unsigned char*)processasn.FTN_OPEN_Form,  12L));

#if 0
  if(status == 0L) {
    status3  = PreFillFile(processasn.Value_Lu);
    if(status3 < 0L) { 
      fprintf(stderr,"Error %3d reported during pre-fill of file %s on LU %d.\n", 
                     status3,processasn.Value_Filename,processasn.Value_Lu);
    }
  }
#endif

  if(processasn.Value_Lu2 >= 0L) {
    (void) process_ftnopen_(&processasn.Value_Lu2,
                             processasn.Value_Filename,
                             processasn.FTN_OPEN_Access,
                             processasn.FTN_OPEN_Status,
                             processasn.FTN_OPEN_Form,
                            &processasn.FTN_OPEN_Recl,
                            &processasn.FTN_OPEN_Readonly,
                            &status2,
                             strlen((char *)processasn.Value_Filename),
                             lnblnk_((unsigned char*)processasn.FTN_OPEN_Access,12L),
                             lnblnk_((unsigned char*)processasn.FTN_OPEN_Status,12L),
                             lnblnk_((unsigned char*)processasn.FTN_OPEN_Form,  12L));

  }

  if((status==125L) || (status2==125L) || (status==126L) || (status2==126L)) {

    fprintf(stderr," Access=%12.12s, Status=%12.12s, Form=%12.12s, Recl=%8d %5d %5d\n",
            processasn.FTN_OPEN_Access,
            processasn.FTN_OPEN_Status,
            processasn.FTN_OPEN_Form,
            processasn.FTN_OPEN_Recl,
            status,status2);

  }

  return(status);
}


/*==================================================================================*/
/* Dettach a file via FORTRAN CLOSE                                                 */
/*==================================================================================*/

static int      DettachLU_Language(int       lu)
{
  int              status;

  (void) process_ftnclose_(&lu, &status);
  return(status);

}

/*==================================================================================*/
/* FORTRAN GetFD_() code - required to support I/O subsystem                        */
/*==================================================================================*/

//int      GetFD_(const int      * lu)
//{
//  int             fd;
//
//  fd    = getfd_((int *)lu);
//  if(fd < 0L) {
//    fd    = private_data.lu_to_fd[*lu];
//  }
//  return(fd);
//
//}

void printbufr(unsigned char * title, unsigned char * buffer, int      length)
{
  int       index;

  fprintf(stderr," %-16s :",title);

  for(index=0;index<length;index++) {
    if((buffer[index] <= ' ') || (buffer[index] > '~')) {
      fprintf(stderr," %02X ",buffer[index]);
    }
    else {
      fprintf(stderr,"  %c ",buffer[index]);
    }
  }
  fprintf(stderr,"\n");
  return;
}

void get_firsttaskargnumb_(int      * index)
{
  (*index) = private_data.first_task_arg;
  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
