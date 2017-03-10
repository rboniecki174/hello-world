/**************************************************************************/
/*                                                                        */
/* inquire_size.c                                               R01-01.00 */
/*                                                                        */
/* ====================================================================== */
/*                                                                        */
/* Status   >=  0  --> Number of records in the file                      */
/*          == -1  --> LU fd not currently assigned via FTN               */
/*          == -2  --> Could not do fstat call.                           */
/*          == -3  --> Can not detirmine the record length                */
/*                                                                        */
/* ====================================================================== */
/*                                                                        */
/* Revision  When        Who   Description                                */
/* ========= =========== ===== ========================================== */
/* R01-01.00 07/Apr/1999 DRLJr Changed references of getfd_ to GetFD_ so  */
/*                             code can be used in both "C" and FORTRAN.  */
/*                             GetFD_() is located in process_asnc.c and  */
/*                             process_asnf.c.  The file process_asnc.c   */
/*                             is used for "C" main programs and the file */
/*                             process_asnf.c is used for FORTRAN main    */
/*                             programs.  For FORTRAN main programs the   */
/*                             routine GetFD_() calls the FORTRAN RTL     */
/*                             routine getfd_() and checks the shared     */
/*                             memory segments reatime1 if getfd_()       */
/*                             returns a -1 value.  For the "C" main      */
/*                             programs the shared memory segment is used */
/*                             to detimine the unix file descriptor based */
/*                             on the supplied logical unit.              */
/*                                                                        */
/**************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
int GetFD_(const int * lu);
void get_lu_recl_(int * Lu, int * Recl);

int      inquire_size_(int      * lu, int      * reclsize )
{
 int            UnixFd;
 int            status;
 int            recl;
 int            reclen;
 int            nrecords;
 struct stat    statbuf;

 UnixFd   = GetFD_(lu); 
 if(UnixFd < 0) {
   return(-1L);
 }

 (void)   get_lu_recl_(lu,&recl);
/* 512 is for cusreclen=512 implies the 512 customer record*/
/* assume that if there is a 512 there the programmer intended to override the default*/
 if((recl < 0L) || (*reclsize == 512)) {  
   reclen = (*reclsize);
 }
 else {
   reclen = recl;
 }

 if(reclen < 0L) {
   return(-3L);
 }

 status = fstat(UnixFd,&statbuf);
 if(status < 0) {
   return(-2L);
 }

 nrecords = statbuf.st_size / reclen;

#if 0
 dLog(DLOG_STDLOG,"lu=%3d, UnixFd=%3d, nrecords=%8d, recl=%5d, reclsize=%5d",
         (*lu),UnixFd,nrecords,recl,(*reclsize));
#endif

 return(nrecords);

}
/****************************************************************/
/*                                                              */
/****************************************************************/

