/********************************************************************/
/*                                                                  */
/* ioerr.c        Get I/O status from SVC 1 Pblk          R00-00.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is a replacement for the ioerr.f and ioerr2.f       */
/* subroutines.                                                     */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  23/Mar/1997  DRLJr     Replacement for ioerr[2].f     */
/* R00-00.01  27/Mar/1997  DRLJr     Add support for conerr()       */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include "sysio.h"
#include <dlogdefs.h>

void ioerr_(iopcb * pblk, int      * iostat);
void ioerr2_(iopcb * pblk, int      * iostat);
void conerr_(iopcb * pblk, int      * iostat);
void get_tname_(char * tname, int      tname_len);

static char fmt[] =
 {" %-8.8s IO Error LU=%3d, FC=%02X, Stat = x'%04X', Errno = %5d."};

/*==================================================================*/
/*                                                                  */
/* ioerr2.f was created to eliminate a problem with recursion.  All */
/* work is now done by ioerr_().  Function ioerr2_() is supplied to */
/* eliminate needing to change FORTRAN code.                        */
/*                                                                  */
/*==================================================================*/

void ioerr2_(iopcb * pblk, int      * iostat)     /* FTN IOERR2()   */
{                                                 /*                */
  ioerr_(pblk,iostat);                            /* Call IOERR()   */
  return;                                         /* Return         */
}                                                 /*                */

/*==================================================================*/
/*                                                                  */
/* conerr.f is a suboutine similiar to ioerr and ioerr2.  The       */
/* subroutine conerr() used SVC 2 Code 7 to write an error message  */
/* to the system console.  Now replaced by ioerr.c.                 */
/*                                                                  */
/*==================================================================*/

void conerr_(iopcb * pblk, int      * iostat)     /* FTN CONERR()   */
{                                                 /*                */
  ioerr_(pblk,iostat);                            /* Call IOERR()   */
  return;                                         /* Return         */
}                                                 /*                */

/*==================================================================*/
/* ioerr_()                                                         */
/*==================================================================*/

void ioerr_(iopcb * pblk, int      * iostat)      /* FTN IOERR()    */
{                                                 /*                */
  int               status;                       /* Copy of status */
  char              taskid[12];                   /* Task name space*/
  int               stat_errno;                   /* Errno value    */

  status         =  pblk->stat;                   /* Get I/O status */
  status        &=  0x0000FFFF;                   /* Keep stat bits */
  if((status == 0x8800) || (status == 0x9000)) {  /* File EOF/EOM ? */
    (*iostat) = -1L;                              /* Return status  */
  }                                               /*                */
  else {                                          /* Other status   */
    (*iostat) = status;                           /* Return status  */
  }                                               /*                */

  if(((*iostat) != 0L) && ((*iostat) != -1L)) {   /* Write message  */
    (void) get_tname_(taskid,8L);                 /* Get taskname   */
    stat_errno = (status & 0x7FFF);               /* Get errno      */
    dLog(DLOG_MAJOR,fmt,taskid,pblk->lu,pblk->fc,status,stat_errno);
  }                                               /*                */
  return;                                         /* Go to caller   */
}                                                 /*                */

/********************************************************************/
/*                                                                  */
/********************************************************************/

