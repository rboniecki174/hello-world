/************************************************************************************/
/*                                                                                  */
/* ttyio.c              Support for CRTs                                  R01-02.00 */
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
/* R01-01.00  DRLJr    08/Dec/1998  Added code to skip the tty state change ioctl's.*/
/* R01-02.00  DRLJr    07/Apr/1999  Changed references of getfd_ to GetFD_ so       */
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
// tt80093    gj       22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/

/*                                                                                              */
/************************************************************************************************/

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "dlogdefs.h"
#include "devicecodes.h"
#include "sysio.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define ACK 0x06

extern void (*enable_handler[])();             /* registered handlers */ 

void check_if_tcp_(int      * Lu, int      * YesNo, int      * UnixFd);
int  tty_image_binary(int      lu, int      fc, int      fd);
void get_lu_dcod_(int      * Lu, int      * Dcode);
int self_trap(int            rc, char     * buffer, int size);
extern void     get_tname_(char *, int     );

static char   ranerr[] = {"%8.8s : *WARNING* : Random I/O to tty ignored (FC=x'%02X', LU=%3d)"};
//static char   prcerr[] = {"%8.8s : *WARNING* : Proceed I/O to tty ignored (FC=x'%02X', LU=%3d)"};
static char   tserr[]  = {"%8.8s : **ERROR** : Test and Set I/O to tty (FC=x'%02X', LU=%3d)"};
static char   ufcerr[] = {"%8.8s : **ERROR** : Unknown Function Code (tty) (FC=x'%02X', LU=%3d)"};
static char   rderr[]  = {"%8.8s : **ERROR** : Read from tty failed, lu = %4d, errno = %4d" };
static char   wrerr[]  = {"%8.8s : **ERROR** : Write to tty failed, lu = %4d, errno = %4d" };
static char   ackerr[] = {"%8.8s : *WARNING* : Read from tty with ack's, lu = %4d" };
static char   taskname[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',0,0,0,0 };

#define TASKNAME() get_tname_(taskname,8L)


void ttyio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                        int      * size, int      * radr)
{ 
 int              status;                          /* General Status Variable      */
 int              dev_code;                        /* Associated Device Code       */
 int              UnixFd;                          /* Associated unix fd           */
 int              Lu;                              /* Local copy of logical unit   */
 int              YesNo;                           /* TCP IP Flag                  */
 int              BytesRead;                       /* Number bytes read from tty   */
 int              BytesWrite;                      /* Number bytes written to tty  */
 char           * Buffer;                          /* Buffer for I/O               */

 int              i;                               /* Misc Loop counter - ACK code */
 int              a;                               /* ACK char count    - ACK code */

 Lu      = *lu;                                  /* Copy the LU         */
 Buffer  = buff;                                 /* Move buffer address */


 (void) check_if_tcp_(&Lu,&YesNo,&UnixFd);       /* Check if tcp        */
 (void) get_lu_dcod_ (&Lu,&dev_code);            /* Get the device code */
 if(UnixFd < 0) {                                /* Openned by FORTRAN  */
   UnixFd = GetFD_(lu);                          /* Get the UNIX fd     */
 }                                               /*                     */

 if(((*func) & 0x80) == 0x80) {                  /* Command function    */
   pcb->stat = 0xC000;                           /* Flag as illegal     */  
   return;                                       /*                     */
 }                                               /*                     */

 if(((*func) & 0x04) == 0x04) {	                 /* Random I/O requested*/
   TASKNAME();                                   /* Get the taskname    */
   dLog(DLOG_MINOR,ranerr,taskname,*func,*lu);    /* Write warning       */
 }                                               /*                     */

#if 0
 if(((*func) & 0x08) == 0x00) {	                 /* Proceed I/O request */
   TASKNAME();                                   /* Get the taskname    */
   dLog(DLOG_MINOR,prcerr,taskname,*func,*lu);    /* Write warning       */
 }                                               /*                     */
#endif

 if(((*func) & 0x11) != 0x00) {                  /* Binary/Image ??     */
   if(dev_code != DCOD_TTYM) {
     status = tty_image_binary(Lu,(*func),UnixFd); /* Make tty change   */
   }
 }                                               /*                     */

 if(((*func) & 0xE0) == 0x40) {                  /* Read from tty       */
   for(;;) {                                     /* Read until were done*/
     BytesRead = read(UnixFd,Buffer,(*size));    /* Read the bytes      */
     if(BytesRead == (*size)) {                  /* Buffer is full      */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesRead;                  /* Set number read     */
       break;                                    /*                     */
     }                                           /*                     */
     else if(BytesRead < 0) {                    /* Error reported      */
       if(errno != EINTR) {                      /* Not a signal intr.  */
         TASKNAME();                             /* Get the taskname    */
         dLog(DLOG_MAJOR,rderr,taskname,Lu,errno);/* Report the error    */
         pcb->stat = 0x8000 | errno;             /* Give an error status*/
         pcb->lxf  = 0;                          /* No data moved       */
         break;                                  /*                     */
       }                                         /*                     */
     }                                           /*                     */
     else {                                      /* Read some data      */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesRead;                  /* Set number read     */
       break;                                    /*                     */
     }                                           /*                     */
   }
   if(BytesRead > 0) {                           /* We have data        */
     for(a = 0; a < BytesRead; a++) {            /* Look for an non-ack */
       if(buff[a] != ACK) {                      /* Not an ACK character*/
         break;                                  /* Leave loop          */
       }                                         /*                     */
     }                                           /*                     */
     if(a != 0) {                                /* At least 1 ACK      */
       TASKNAME();                               /* Get taskname        */
       dLog(DLOG_WARNING,ackerr,taskname,Lu);       /* Write Info message  */
       for(i=0; a < BytesRead;) {                /* Move other data     */
         buff[i++] = buff[a++];                  /* over ACKs           */
       }                                         /*                     */
       pcb->lxf = i;                             /* Update lxf field    */
     }                                           /*                     */
   }                                             /*                     */
 }                                               /* End of the read     */
 else if(((*func) & 0xE0) == 0x20) {             /* Write to tty        */
   for(;;) {                                     /* Read until were done*/
     BytesWrite = write(UnixFd,Buffer,(*size));  /* Write the bytes     */
     if(BytesWrite == (*size)) {                 /* Buffer is written   */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesWrite;                 /* Set number written  */
       break;                                    /*                     */
     }                                           /*                     */
     else if(BytesWrite < 0) {                   /* Error reported      */
       if(errno != EINTR) {                      /* Not a signal intr.  */
         TASKNAME();                             /* Get the taskname    */
         dLog(DLOG_MAJOR,wrerr,taskname,Lu,errno);/* Report the error    */
         pcb->stat = 0x8000 | errno;             /* Give an error status*/
         pcb->lxf  = 0;                          /* No data moved       */
         break;                                  /*                     */
       }                                         /*                     */
     }                                           /*                     */
     else {                                      /* Read some data      */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesWrite;                 /* Set number written  */
       break;                                    /*                     */
     }                                           /*                     */
   }
 }
 else if(((*func) & 0xE0) == 0x60) {             /* Test and set        */
   TASKNAME();                                   /* Get the task name   */
   dLog(DLOG_MAJOR,tserr,taskname,(*func),Lu);    /* Report the errorr   */
   pcb->stat  = 0xC000;                          /* Illegal Function    */
 }                                               /*                     */
 else {                                          /* Not valid           */
   TASKNAME();                                   /* Get the task name   */
   dLog(DLOG_MAJOR,ufcerr,taskname,(*func),Lu);   /* Report the errorr   */
   pcb->stat  = 0xC000;                          /* Illegal Function    */
 }                                               /*                     */

 /* If we are doing a proceed I/O request then generate an trap to the  */
 /* I/O Completion Handler Code                                         */

 if(((*func & 0x08) == 0) && (enable_handler[4] != 0)) {
   (void) self_trap(0x08,(void *)&pcb,4L);
 }

 /* Reset the terminal to 'normal' mode if I/O was image or binary      */

 if(((*func)&0x11) != 0x00) {                    /* Binary/Image ??     */
   if(dev_code != DCOD_TTYM) {
     status = tty_image_binary(Lu,0x00,UnixFd);  /* Make tty change     */
   }
 }                                               /*                     */

 return;                                         /* Return to caller    */
}                                                /* ttyio               */

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

