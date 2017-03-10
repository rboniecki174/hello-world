#include "lock_port.h"
#include "garid_.h"
#include "sysdb.h"
#include "dlogdefs.h"
#include "proto_common.h"

//-------------------------------------------------------------------------------------
// dksysio_()
//-------------------------------------------------------------------------------------
//THIS SUBROUTINE DOES A SYSIO TO DISK WITH RECOVERY FEATURES.
//IT CALLS MIGFIL TO SEE IF A FILE MIGRATION IS NECESSARY.
//IT DOES THE I/O IF THE ERROR COUNT IS BELOW THE SPECIFIED LIMIT.
//IT INCREMENTS THE ERROR COUNT IF NECESSARY.
// ARGUMENTS:  SEE THE FORTRAN USER GUIDE FOR SYSIO
//-------------------------------------------------------------------------------------
// REV    DATE           DESCRIPTION
// ---   ------           ----------
//  0       10-01-83     INITIAL RELEASE
//  V49.01  07-21-94     If error limit exceeded, issue warning
//  tt79818 21-Apr-2014  Convert to C to remove ftn from libos32
//-------------------------------------------------------------------------------------

void ioerr_(iopcb * pblk, int      * iostat);

void dksysio_(int pblk[],
             const int *fc,
             const int *lu, 
             const void *start,
             const int *nbytes,   
             const int *ranadd)
{
     int    iostatus;
     sysio_((iopcb *)pblk,fc,lu,(void *)start,nbytes,ranadd);
     ioerr_((iopcb *)pblk, &iostatus);  //CHECK ERRORS
//   if (iostatus != 0) DSKERRCN = DSKERRCN + 1
//   if (dskerrcn > dskerrlm) {
//     dLog(DLOG_MAJOR,"DKSYSIO: DSKERRLM EXCEEDED - RESETTING"););
//     dskerrcn = 0;
//   }
   return;
}
