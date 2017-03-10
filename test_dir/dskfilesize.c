/**************************************************************************/
/*                                                                        */
/* dskfilesize.c Detirmine number of records/sectors in a file  R00-01.00 */
/*                                                                        */
/* ====================================================================== */
/*                                                                        */
/* Revision  When        Who   Description                                */
/* ========= =========== ===== ========================================== */
/* R00-01.00 07/Apr/1999 DRLJr Changed references of getfd_ to GetFD_ so  */
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
// tt80093   24/Jun/2014 gj    Clean up prototypes
/**************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <termio.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include "dlogdefs.h"
#include "devicecodes.h"
#include "sysio.h"
#include "proto_common.h" //tt80093

//int GetFD_(const int * lu);                           //tt80093
//void get_lu_fd_(int * Lu, int * YesNo, int * UnixFd); //tt80093
/*========================================================================*/
/* Formats and other static data                                          */
/*========================================================================*/

static char fmt_position[] = {
 "**ERROR** dskfilesize.c : Error occured during file sizing (lu=%4d)" };
static char fmt_reclsize[] = {
 "*WARNING* dskfilesize.c : File Size is not multiple of record length (lu=%4d)" };

/*========================================================================*/
/* Routine dskfilesize_()                                                 */
/*========================================================================*/

void dskfilesize_(int      * lu,   int      * dcode, int      * recl, 
                  int      * nrecs)
{
 int           UnixFd;                   /* Primary File's fd             */
 int           YesNo;                    /* TCP/IP Flag                   */
 int           nrecords;                 /* Number of records             */
 int           remainder;                /* Left over bytes               */
 off_t         curr_offs;                /* Current Offset in file        */
 off_t         last_offs;                /* Current Offset in file        */
 off_t         rest_offs;                /* Restore offset value          */

#if 0
 dLog(DLOG_STDLOG,"Lu=%4d, Dcode=%4d, Recl=%4d\n",(*lu),(*dcode),(*recl));
#endif

 (void) get_lu_fd_(lu,&YesNo,&UnixFd);   /* Get the Unix FD for I/O       */
 if(UnixFd < 0) {                        /* Open outside of process_asn ? */
   UnixFd = GetFD_(lu);                  /* Get the UNIX fd from FORTRAN  */
 }                                       /*                               */

 if(UnixFd < 0) {                        /* Must not be assigned          */
   nrecords  = -1L;                      /* Indicate not assign           */
 }                                       /*                               */
 else {                                  /* Get the current offset        */
   if(((*dcode) == DCOD_CONTIG) || ((*dcode) == DCOD_INDEX)) {
     curr_offs = lseek(UnixFd,(off_t)        0L,SEEK_CUR);
     last_offs = lseek(UnixFd,(off_t)        0L,SEEK_END);
     rest_offs = lseek(UnixFd,(off_t) curr_offs,SEEK_SET);
     if((curr_offs != rest_offs) || (rest_offs < (off_t) 0)) {
       dLog(DLOG_MAJOR,fmt_position,(*lu));
       nrecords  =  -1L;
       remainder =   0L;
     }
     else if((*dcode) == DCOD_CONTIG) {
       nrecords  =  last_offs / SECTOR;
       remainder =  last_offs % SECTOR;
     }
     else if((*dcode) == DCOD_INDEX) {
       nrecords  = last_offs / (*recl);
       remainder = last_offs % (*recl);	
       if(nrecords < 0L) {
         nrecords  = -1L;
         remainder =  0L;
       }
     }
   }
   else {
     nrecords  = -1L;
     remainder =  0L;
   }
 }                                       /*                               */
 if(remainder != 0L) {
   dLog(DLOG_MINOR,fmt_reclsize,(*lu));
 }

 (*nrecs) = nrecords;                    /* Return the number of records  */

#if 0
 dLog(DLOG_STDLOG,"dskfilesize.c : nrecords = %8d, remainder = %8d",
                 nrecords,remainder);
#endif
 
 return;                                 /* Return to the caller          */

}                                        /*                               */

/*========================================================================*/
/* Routine dskfilesize()                                                  */
/*========================================================================*/

void dskfilesize(int      lu, int      dcode, int      recl, int      * nrecs)
{
  dskfilesize_(&lu,&dcode,&recl,nrecs);
  return;
}

/**************************************************************************/
/*                                                                        */
/**************************************************************************/
