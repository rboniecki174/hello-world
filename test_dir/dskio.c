/**************************************************************************************/
/*                                                                                    */
/* dskio.c                       Support for Files                          R01-01.01 */
/*                                                                                    */
/* Revision History:                                                                  */
/* ================================================================================== */
/* 15/Aug/1997 DRLJr Changed the random I/O control flag byteaddr to use -1L to       */
/*                   indicate a sequential I/O versus a 0L.  This problem was seen    */
/*                   because the file MARPFILE.DAT makes use of sector 0 and the      */
/*                   other files CUSTFILE.DAT and ISSUFILE.DAT do not make use of     */
/*                   sector 0.                                            (R01-00.01) */
/*                                                                                    */
/* 08/Oct/1997 DRLJr Made changes to support random I/O to any type of file and allow */
/*                   for I/O to handle buffers that may be shorter or longer than     */
/*                   record length of the specific file.                              */
/*                                                                                    */
/* 16/Oct/1997 DRLJr Added a REWIND call to replace the FORTRAN REWIND statement().   */
/*                                                                                    */
/* 26/Nov/1997 DRLJr Changed to not generate an error if offset is 0 doing a          */
/*                   backrecord.                                                      */
/*                                                                                    */
/* 08/Mar/1999 DRLJr Added mirror support for files that are assigned with the mirror */
/*                   keyword.  Mirrored I/O errors are reported to stderr but do not  */
/*                   errors in the parameter block status field.  Did some clean-up   */
/*                   to make the file easier to edit and review.  Added code to       */
/*                   insure that index files are expand by 1 record at a time.        */
/*                                                                     R01-01.00      */
/*                                                                                    */
/* 14/May/1999 DRLJr Commented out some debug messages (R01-01.01)                    */
//
// 22/May/2014 gj    tt80093 - Clean up prototypes
/*                                                                                    */
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sched.h>
#include <termio.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
//#include "libos32.h"    //tt80093
#include "dlogdefs.h"
#include "proto_common.h" //tt80093
#include "devicecodes.h"
#include "sysio.h"

#define  DEBUGG  0

#define  ACK    0x06

extern void (*enable_handler[])();             /* registered handlers */ 
static char   prcerr[] =
  {"%8.8s : *WARNING* : Proceed I/O to dsk ignored (FC=x'%02X', LU=%3d)\n"};
static char   tserr[]  = 
  {"%8.8s : **ERROR** : Test and Set I/O to dsk (FC=x'%02X', LU=%3d)\n"};
static char   ufcerr[] =
  {"%8.8s : **ERROR** : Unknown Function Code (dsk) (FC=x'%02X', LU=%3d, STAT=%04X)\n"};
static char   rderr[]  =
  {"%8.8s : **ERROR** : Read from dsk failed, lu = %4d, errno = %4d\n" };
static char   wrerr[]  = 
  {"%8.8s : **ERROR** : Write to dsk failed, lu = %4d, errno = %4d\n" };
static char   ranerr[] =
 {"%8.8s : **ERROR** : Read/Write at %d exceeds file size of %d on lu %d.\n" };
static char   ranerr0[] =
 {"%8.8s : **ERROR** : Read/Write at %d before first file record (size = %d, lu = %d).\n" };
static char   ranerr1[] =
 {"%8.8s : **ERROR** :    (nextrec=%d, nrecs=%d, dcode=%d CO=size-1, IN=size+1\n" };
static char   unassgn[] =
 {"%8.8s : **ERROR** : Logical Unit %d is not assigned.  Status=%04X, Func=%02X." };
static char   mirrerr[] =
 {"%8.8s : **ERROR** : Mirror I/O Error, lu = %4d, Func = %02X, Errno = %4d." };

static char   taskname[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',0,0,0,0 };

#define TASKNAME()  if(taskname[0] == ' ') get_tname_(taskname,8L)

int      alloc_iobuffer(int        func,   int        dcode,  int        reclen,
                        int        length, char     * buffer, char    ** iobuff,
                        int      * xferusr, int      * nrecs);

void get_lu_fd_(int      * Lu, int      * YesNo, int      * UnixFd);
void set_lu_size_(int      * Lu, int      * Size);
void get_lu_recl_(int      * Lu, int      * Recl);
void get_lu_dcod_(int      * Lu, int      * Dcode);
void get_mirr_fd_(int      * Lu, int      * UnixFd);
void set_lu_size_(int      * Lu, int      * Size);
void get_lu_size_(int      * Lu, int      * Size);
int self_trap(int            rc, char     * buffer, int size);

/*===============================================================================*/
/*                                                                               */
/*===============================================================================*/

void dskio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                        int      * size, int      * radr)
{ 
 int              dev_code;                      /* Associated Device Code       */
 int              UnixFd;                        /* Associated unix fd           */
 int              MirrFd;                        /* Associated unix fd (mirror)  */
 int              Lu;                            /* Local copy of logical unit   */
 int              YesNo;                         /* TCP IP Flag                  */
 int              BytesRead;                     /* Number bytes read from tty   */
 int              BytesWrite;                    /* Number bytes written to tty  */
 char           * Buffer;                        /* Buffer for I/O               */
 int              byteaddr;                      /* Byte address of data to read */
 off_t            seek_loc;                      /* Seek Location                */
 off_t            curr_loc;                      /* Current Location             */
 off_t            mirr_loc;                      /* Current Location             */
 int              reclen;                        /* Record length of the file    */
 int              xferlen;                       /* Amount of data transfered    */
 int              xferusr;                       /* Amount data moved for user   */
 int              filesize;                      /* Size of contiguous files     */
 int              nextrec;                       /* Next Record value            */
 int              nrecs;                         /* Number of records in I/O     */

 Lu      = *lu;                                  /* Copy the LU                  */
 Buffer  = buff;                                 /* Move buffer address          */
 
 (void) get_lu_fd_(lu,&YesNo,&UnixFd);           /* Get the Unix FD for I/O      */
 if(UnixFd < 0) {                                /* Open outside of process_asn? */
   UnixFd = GetFD_(lu);                          /* Get the UNIX fd from FORTRAN */
 }                                               /*                              */

 if(UnixFd < 0) {                                /* Must not be asisgned         */
   TASKNAME();                                   /* Set the taskname             */
   pcb->stat = 0x8100 + Lu;                      /* Set LU unassigned            */
   dLog(DLOG_MAJOR,unassgn,taskname,(*lu),(pcb->stat&0xFFFF),((*func)&0xFF));
   return;                                       /* Exit Processing              */
 }                                               /*                              */

 (void) get_lu_recl_(lu,&reclen);                /* Get the record length to use */
 (void) get_lu_dcod_(lu,&dev_code);              /* Get the device code          */
 (void) get_lu_size_(lu,&filesize);              /* Get the file size            */
 (void) get_mirr_fd_(lu,&MirrFd);                /* Get the mirror file unix fd  */
 
 curr_loc = lseek(UnixFd,(off_t) 0L,SEEK_CUR);   /* Location of where we are     */
 if(filesize > 0) {                              /* Avoid divide by zero         */
   nextrec  = curr_loc / filesize;               /* Record Number of file        */
 }                                               /*                              */
 else {                                          /*                              */
   nextrec  = 0L;                                /*                              */
 }                                               /*                              */
 mirr_loc   = curr_loc;                          /* Save location for I/O        */

 /*==============================================================================*/
 /*                                                                              */
 /* Check if we are doing a command function.  If so then perform the command    */
 /* function and exit at this point and then return to the caller                */
 /*                                                                              */
 /*==============================================================================*/

 if(((*func) & 0x80) == 0x80) {                  /* Command Function             */
   switch((*func)) {                             /* Process Command              */
     case 0xC0 :                                 /* Rewind                       */
                 seek_loc = lseek(UnixFd,(off_t) 0L,SEEK_SET); /* Set position   */
                 if(seek_loc < (off_t) 0) {      /* An error                     */
                   pcb->stat = 0x8400;           /* Generate an error            */
                 }                               /*                              */
                 else {                          /* Good status                  */
                   pcb->stat = 0x0000;           /* Set status                   */
                 }                               /*                              */
                 break;                          /* Leave switch                 */
     case 0xA0 :                                 /* Backspace Record             */
                 if(reclen < 1L) {               /* Record length not set?       */
                   pcb->stat = 0xC100 + Lu;      /* Indicate problem             */
                 }                               /*                              */
                 else {                          /* Move back 1 record           */
                   reclen *= (-1L);              /* Negative displace            */
                   if(curr_loc != (off_t) 0) {   /* Currently at zero >          */
                     seek_loc = lseek(UnixFd,(off_t) reclen,SEEK_CUR); /* No,Seek*/
                   }                             /*                              */
                   else {                        /*                              */
                     seek_loc = (off_t) 0;       /*                              */
                   }                             /*                              */
                   if(seek_loc < (off_t) 0) {    /* An error                     */
                     pcb->stat = 0x8400;         /* Generate an error            */
                   }                             /*                              */
                   else {                        /* Good status                  */
                     pcb->stat = 0x0000;         /* Set status                   */
                   }                             /*                              */
                 }                               /*                              */
                 break;                          /* Leave switch                 */
     case 0x90 :                                 /* Forwardspace Record          */
                 if(reclen < 1L) {               /* Record len not set?          */
                   pcb->stat = 0xC100 + Lu;      /* Indicate problem             */
                 }                               /*                              */
                 else {                          /* Move back 1 record           */
                   seek_loc = lseek(UnixFd,(off_t) reclen,SEEK_CUR); /* Seek     */
                   if(seek_loc < (off_t) 0) {    /* An error                     */
                     pcb->stat = 0x8400;         /* Generate an error            */
                   }                             /*                              */
                   else {                        /* Good status                  */
                     pcb->stat = 0x0000;         /* Set status                   */
                   }                             /*                              */
                 }                               /*                              */
                 break;                          /* Leave switch                 */
     case 0x88 :                                 /* Write Filemark               */
                 pcb->stat = 0xC000;             /* Illegal Function             */
                 break;                          /* Leave switch                 */
     case 0x84 :                                 /* Forward Filemark             */
                 seek_loc = lseek(UnixFd,(off_t) 0L,SEEK_END); /* Seek           */
                 seek_loc = lseek(UnixFd,(off_t) 0L,SEEK_CUR); /* Seek           */
               /*dLog(DLOG_MAJOR,"FFM lseek() = %08X %12d\n",seek_loc,seek_loc);*/
                 if(seek_loc < (off_t) 0) {      /* An error                     */
                   pcb->stat = 0x8400;           /* Generate an error            */
                 }                               /*                              */
                 else {                          /* Good status                  */
                   pcb->stat = 0x0000;           /* Set status                   */
                 }                               /*                              */
                 break;                          /* Leave switch                 */
     case 0x82 :                                 /* Backward Filemark            */
                 seek_loc = lseek(UnixFd,(off_t) 0L,SEEK_SET); /* Seek           */
                 if(seek_loc < (off_t) 0) {      /* An error                     */
                   pcb->stat = 0x8400;           /* Generate an error            */
                 }                               /*                              */
                 else {                          /* Good status                  */
                   pcb->stat = 0x0000;           /* Set status                   */
                 }                               /*                              */
                 break;                          /* Leave switch                 */
     case 0x81 :                                 /* Device Dependent             */
                 pcb->stat = 0xC000;             /* Illegal Function             */
                 break;                          /* Leave switch                 */
     case 0x80 :                                 /* Halt I/O                     */
                 pcb->stat = 0xC000;             /* Illegal Function             */
                 break;                          /* Leave switch                 */
     default   :                                 /* Unknown                      */
                 pcb->stat = 0xC000;             /* Illegal Function             */
                 break;                          /* Leave switch                 */
   }                                             /*                              */
   if((pcb->stat & 0xFF00) != 0x0000) {          /*                              */
     TASKNAME();                                 /* Get the task name            */
     dLog(DLOG_MAJOR,ufcerr,taskname,(*func),Lu,(pcb->stat & 0xFFFF)); 
   }                                             /*                              */
   if(MirrFd >= 0L) {                            /* Insure files are in sync     */
     errno    = -1L;                             /* Clear errno value            */
     curr_loc = lseek(UnixFd,(off_t) 0L,SEEK_CUR); /* Get the current location   */
                                                 /* of the primary file          */
     if(curr_loc >= 0L) {                        /* Set the mirror files         */
       errno    = -2L;                           /* Indicate processing          */
       seek_loc = lseek(MirrFd,curr_loc,SEEK_SET); /* Do the set operation       */
       if((seek_loc != curr_loc) || (seek_loc < (off_t) 0L)) {
         dLog(DLOG_MAJOR,mirrerr,taskname,(*lu),(*func),errno);
       }
     }                                           /*                              */
     else {                                      /* Initial seek failed          */
       dLog(DLOG_MAJOR,mirrerr,taskname,(*lu),(*func),errno);
     }                                           /*                              */
   }                                             /*                              */
   return;                                       /* Return to caller             */
 }                                               /*                              */

 /*==============================================================================*/
 /*                                                                              */
 /* Check if we are doing a proceed I/O and generate a warning.  The enable      */
 /* system supports proceed I/O but UNIX in general does not support a true      */
 /* proceed I/O operation unless POSIX real-time is supported completely.        */
 /*                                                                              */
 /*==============================================================================*/

 if(((*func) & 0x08) == 0x00) {	                 /* Proceed I/O request */
   TASKNAME();                                   /* Get the taskname    */
   dLog(DLOG_MINOR,prcerr,taskname,*func,*lu);    /* Write warning       */
 }                                               /*                     */

 /*==============================================================================*/
 /*                                                                              */
 /* Perform the set-up needed for random I/O operations.                         */
 /*                                                                              */
 /*==============================================================================*/

 /* Note: The selection of random I/O only works on SECTOR (Contiguous) */
 /*       files at this time.  To work for INDEXed type files a record  */
 /*       must be supplied from another source.  (OBSOLETE-08/Mar/1999) */

 /* Note: The buffer that is used is expected to be a complete record   */
 /*       or an even number of SECTORs to insure that file positioning  */
 /*       is correct.                            (OBSOLETE-08/Mar/1999) */

 /* Note: This code will now allow for RANDOM I/O to both "CONTIGUOUS"  */
 /*       and "INDEXED" files provided a "record length" is provided to */
 /*       the routine.  If the record length is not provided then a     */
 /*       record length of 256 (i.e. SECTOR) will be used as the record */
 /*       length of the file.                      DRLJr 07/Oct/1997    */

 if(((*func) & 0x04) == 0x04) {                  /* Random I/O request  */
   if((*radr) < 0L) {                            /* Record count < 0L ? */
     TASKNAME();                                 /* Get the taskname    */
     if(dev_code == DCOD_CONTIG) {               /* Report EOM for CO   */
       pcb->stat = 0x9000;                       /* End-of-Medium       */
       pcb->lxf  = 0x0000;                       /*                     */
     }                                           /*                     */
     else if(dev_code == DCOD_INDEX) {           /* Report EOF for IN   */
       pcb->stat = 0x8800;                       /* End-of-File         */
       pcb->lxf  = 0x0000;                       /*                     */
     }                                           /*                     */
     else {                                      /* File is not set     */
       pcb->stat = 0x88FF;                       /* End-of-File         */
       pcb->lxf  = 0x0000;                       /*                     */
     }                                           /*                     */
     dLog(DLOG_MAJOR,ranerr0,taskname,(*radr),filesize,(*lu));
     dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
     goto exit;                                  /*                     */
   }                                             /*                     */
   if(reclen < 0L) {                             /* Assume 256 bytes    */
     byteaddr = (*radr) * SECTOR;                /* Byte location to rd */
     nextrec  = byteaddr / SECTOR;               /* Sector for I/O      */
   }                                             /*                     */
   else if(reclen == 0L) {                       /* Treat as byte file  */
     byteaddr = (*radr);                         /* Use value offset    */
     nextrec  = byteaddr;                        /* Byte location       */
   }                                             /*                     */
   else {                                        /* Use record length   */
     byteaddr = (*radr) * reclen;                /* Offset into file    */
     nextrec  = byteaddr / reclen;               /* Record for I/O      */
   }                                             /*                     */
 }                                               /*                     */
 else {                                          /* Sequential I/O      */
   byteaddr = -1L;                               /* Set as a flag       */
   if(reclen > 0) {                              /* Must be positive    */
     nextrec  = curr_loc / reclen;               /* The next record     */
   }                                             /*                     */
   else {                                        /*                     */
     nextrec  =  0L;                             /*                     */
   }                                             /*                     */
 }                                               /*                     */

#if DEBUGG
 if((*lu) == 1L) {
   dLog(DLOG_MAJOR,"byteaddr = %08X  %8d  %o10.\n",byteaddr,byteaddr,byteaddr);
 }
#endif

 /*==============================================================================*/
 /*                                                                              */
 /* Perform a read operation from a file.                                        */
 /*                                                                              */
 /*==============================================================================*/

 if(((*func) & 0x60) == 0x40) {                  /* Read from file      */

   xferlen = alloc_iobuffer((*func),dev_code,reclen,(*size),buff,&Buffer,
                            &xferusr,&nrecs);

   if((filesize >= 0L) && (reclen > 0L)) {
     if(dev_code == DCOD_CONTIG) {
       if((nextrec + nrecs) > filesize) {
         TASKNAME();
         if(((*func) & 0x04) == 0x04) {                  /* Random I/O request  */
           dLog(DLOG_MAJOR,ranerr,taskname,(*radr),filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         else {
           dLog(DLOG_MAJOR,ranerr,taskname,nextrec,filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         pcb->stat = 0x9000;
         pcb->lxf  = 0x0000;
         goto exit;
       }
     }
     else if(dev_code == DCOD_INDEX) {
       if((nextrec + nrecs) > filesize + 1L) {
         TASKNAME();
         if(((*func) & 0x04) == 0x04) {                  /* Random I/O request  */
           dLog(DLOG_MAJOR,ranerr,taskname,(*radr),filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         else {
           dLog(DLOG_MAJOR,ranerr,taskname,nextrec,filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         pcb->stat = 0x8800;
         pcb->lxf  = 0x0000;
         goto exit;
       }
     
     }
   }

   if(xferlen > 0) {
     for(;;) {                                     /* Read until were done*/
       if(byteaddr >= 0) {
         BytesRead  = lseek(UnixFd,(off_t) byteaddr,SEEK_SET);
         if(BytesRead >= 0L) {
           BytesRead = read(UnixFd,Buffer,xferlen);
         }
       }
       else {
         BytesRead = read(UnixFd,Buffer,xferlen);  /* Read the bytes      */
       }
       if(BytesRead == xferlen) {                  /* Buffer is full      */
         pcb->stat  =  0x0000;                     /* Set good status     */
         pcb->lxf   =  xferusr;                    /* Set number read     */
         (void) memcpy(buff,Buffer,xferusr);       /* Move the data       */
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
       else if(BytesRead == 0) {                   /* EOF/EOM status      */
         pcb->stat  = 0x8800;                      /* Set EOF status      */
         pcb->lxf   = 0;                           /* No bytes moved      */
         break;                                    /* We are done         */
       }                                           /*                     */
       else {                                      /* Read some data      */
         pcb->stat  =  0x0000;                     /* Set good status     */
         if(BytesRead > xferusr) {                 /* Got user's amount   */
           pcb->lxf =  xferusr;                    /* Indicate this much  */
         }                                         /*                     */
         else {                                    /* Indicate smaller    */
           pcb->lxf =  BytesRead;                  /* Set number read     */
         }                                         /*                     */
         (void) memcpy(buff,Buffer,pcb->lxf);      /* Move the data       */
         break;                                    /*                     */
       }                                           /*                     */
     }                                             /*                     */
     (void) free(Buffer);                          /* Release work buffer */
   }
   else {
     pcb->stat = 0x8400;
     pcb->lxf  = 0L;
   }
   if(MirrFd >= 0L) {                              /* Insure files are in sync   */
     errno    = -1L;                               /* Clear errno value          */
     curr_loc = lseek(UnixFd,(off_t) 0L,SEEK_CUR); /* Get the current location   */
                                                   /* of the primary file        */
     if(curr_loc >= 0L) {                          /* Set the mirror files       */
       errno    = -2L;                             /* Indicate processing        */
       seek_loc = lseek(MirrFd,curr_loc,SEEK_SET); /* Do the set operation       */
       if((seek_loc != curr_loc) || (seek_loc < (off_t) 0L)) {
         dLog(DLOG_MAJOR,mirrerr,taskname,(*lu),(*func),errno);
       }
     }                                             /*                            */
     else {                                        /* Initial seek failed        */
       dLog(DLOG_MAJOR,mirrerr,taskname,(*lu),(*func),errno);
     }                                             /*                            */
   }                                               /*                            */
 }                                                 /* End of the read            */

 /*==============================================================================*/
 /*                                                                              */
 /* Perform a write operation to a file.                                         */
 /*                                                                              */
 /*==============================================================================*/

 else if(((*func) & 0x60) == 0x20) {                   /* Write to a file        */

   xferlen = alloc_iobuffer((*func),dev_code,reclen,(*size),buff,&Buffer,
                            &xferusr,&nrecs);

   if((filesize >= 0L) && (reclen > 0L)) {
     if(dev_code == DCOD_CONTIG) {
       if((nextrec + nrecs) > filesize) {
         TASKNAME();
         if(((*func) & 0x04) == 0x04) {                  /* Random I/O request  */
           dLog(DLOG_MAJOR,ranerr,taskname,(*radr),filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         else {
           dLog(DLOG_MAJOR,ranerr,taskname,nextrec,filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         pcb->stat = 0x9000;
         pcb->lxf  = 0x0000;
         goto exit;
       }
     }
     else if(dev_code == DCOD_INDEX) {
       if((nextrec + nrecs) > filesize + 1L) {
         TASKNAME();
         if(((*func) & 0x04) == 0x04) {                  /* Random I/O request  */
           dLog(DLOG_MAJOR,ranerr,taskname,(*radr),filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         else {
           dLog(DLOG_MAJOR,ranerr,taskname,nextrec,filesize,(*lu));
           dLog(DLOG_MAJOR,ranerr1,taskname,nextrec,nrecs,dev_code);
         }
         pcb->stat = 0x8800;
         pcb->lxf  = 0x0000;
         goto exit;
       }
     }
   }
   if(xferlen > 0) {
     for(;;) {                                     /* Write until done   */
       if(byteaddr >= 0) {
         BytesWrite  = lseek(UnixFd,(off_t) byteaddr,SEEK_SET);
         if(BytesWrite >= 0L) {
           BytesWrite = write(UnixFd,Buffer,xferlen);
         }
       }
       else {
         BytesWrite = write(UnixFd,Buffer,xferlen);/* Write the bytes     */
       }
       if(BytesWrite == xferlen) {                 /* Buffer is written   */
         pcb->stat  =  0x0000;                     /* Set good status     */
         pcb->lxf   =  xferusr;                    /* Set number written  */
         if(MirrFd >= 0L) {
           if(byteaddr >= 0) {
             BytesWrite  = lseek(MirrFd,(off_t) byteaddr,SEEK_SET);
             if(BytesWrite >= 0L) {
               BytesWrite = write(MirrFd,Buffer,xferlen);
             }
           }
           else {
             BytesWrite = lseek(MirrFd,mirr_loc,SEEK_SET);
             BytesWrite = write(MirrFd,Buffer,xferlen);/* Write the bytes     */
           }
           if(BytesWrite < 0L) {
             dLog(DLOG_MAJOR,mirrerr,taskname,(*lu),(*func),errno);
           }
         }
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
       else if(BytesWrite == 0) {                  /* EOF/EOM status      */
         pcb->stat  = 0x8800;                      /* Set EOF status      */
         pcb->lxf   = 0;                           /* No bytes moved      */
       }                                           /*                     */
       else {                                      /* Read some data      */
         pcb->stat  =  0x8201;                     /* Set error status    */
         if(BytesWrite > xferusr) {                /* Got user's amount   */
           pcb->lxf =  xferusr;                    /* Indicate this much  */
         }                                         /*                     */
         else {                                    /* Indicate smaller    */
           pcb->lxf =  BytesWrite;                 /* Set number read     */
         }                                         /*                     */
         break;                                    /*                     */
       }                                           /*                     */
     }                                             /*                     */
     if((dev_code == DCOD_INDEX) && (filesize >= 0L)) {  /* Index file ?  */
#if 0
       dLog(DLOG_MAJOR,"Expanding record count - nextrec = %5d, nrecs = %5d, filesize = %5d\n",
                       nextrec,nrecs,filesize);
#endif
       if((nextrec + nrecs) == filesize + 1L) {    /* Expanded the file ? */
         filesize++;                               /* Increment count     */
         (void) set_lu_size_(lu,&filesize);        /* Update tables       */
       }                                           /*                     */
     }                                             /*                     */
     (void) free(Buffer);                          /* Release work buffer */
   }
   else {
     pcb->stat = 0x8400;
     pcb->lxf  = 0L;
   }
 }

 /*==============================================================================*/
 /*                                                                              */
 /* Test and Set Operations are not supported.                                   */
 /*                                                                              */
 /*==============================================================================*/

 else if(((*func) & 0x60) == 0x60) {             /* Test and set        */
   TASKNAME();                                   /* Get the task name   */
   pcb->stat  = 0xC000;                          /* Illegal Function    */
   dLog(DLOG_MAJOR,tserr,taskname,(*func),Lu);    /* Report the errorr   */
 }                                               /*                     */

 /*==============================================================================*/
 /*                                                                              */
 /* Function Code is not supported.                                              */
 /*                                                                              */
 /*==============================================================================*/

 else {                                          /* Not valid           */
   TASKNAME();                                   /* Get the task name   */
   pcb->stat  = 0xC000;                          /* Illegal Function    */
   dLog(DLOG_MAJOR,ufcerr,taskname,(*func),Lu,(pcb->stat & 0xFFFF));
 }                                               /*                     */

 /*==============================================================================*/
 /*                                                                              */
 /* Exit Code and clean-up processing.                                           */
 /*                                                                              */
 /*==============================================================================*/

 exit:

 /* If we are doing a proceed I/O request then generate an trap to the  */
 /* I/O Completion Handler Code                                         */

 if(((*func & 0x08) == 0) && (enable_handler[4] != 0)) {
   (void) self_trap(0x08,(char *)&pcb,4L);
 }

 return;                                         /* Return to caller    */
}                                                /* ttyio               */

/*===========================================================================*/
/* alloc_iobuffer - Allocate Buffer for I/O Operation                        */
/*===========================================================================*/

int      alloc_iobuffer(int      func,   int        dcode,  int        reclen,
                        int      length, char     * buffer, char    ** iobuff,
                        int      * xferusr, int      * records)
{
  int         alloc_size;                        /* Size of buffer to alloc  */
  int         mov_bytes;                         /* Move bytes for writes    */
  int         nrecs;                             /* Number of records        */
  char      * wrkbuff;                           /* Work buffer              */

  if(dcode == DCOD_CONTIG) {                     /* CONTIGUOUS type file     */
    alloc_size = ((length+SECTOR-1)/SECTOR)*SECTOR; /* Size of work buffer   */
    mov_bytes  = length;                         /* Bytes to move for I/O    */
    *xferusr   = length;                         /* Data moved for user      */
    nrecs      = alloc_size / SECTOR;            /* Number of sectors        */
  }                                              /*                          */
  else if(dcode == DCOD_INDEX) {                 /* INDEX type file          */
    alloc_size = reclen;                         /* Size of the work buffer  */
    mov_bytes  = ((length>reclen)?reclen:length);/* Amount of data to move   */
    *xferusr   = mov_bytes;                      /* Data moved for user      */
    nrecs      = 1L;                             /* Number of records        */
  }                                              /*                          */
  else {                                         /* Not INDEX or CONTIGUOUS  */
    alloc_size = length;                         /* Size of the work buffer  */
    mov_bytes  = length;                         /* Bytes to move for I/O    */
    *xferusr   = mov_bytes;                      /* Data moved for user      */
    nrecs      = alloc_size / SECTOR;            /* Number Sectors           */
  }                                              /*                          */
  if(reclen <= 0) {                              /* Not set - use as is      */
    alloc_size = length;                         /* Size of work buffer      */
    mov_bytes  = length;                         /* Bytes to move for I/O    */
    *xferusr   = mov_bytes;                      /* Data moved for user      */
    nrecs      = length;                         /* Number of bytes          */
  }                                              /*                          */

  wrkbuff = (char *) malloc(alloc_size);         /* Allocate work buffer     */
  if(wrkbuff == (char *) NULL) {                 /* Was buffer acquired?     */
    alloc_size = 0L;                             /* Indicate error           */
    goto exit;                                   /* Indicate error           */
  }                                              /*                          */

  if((func & 0x10) == 0x10) {                    /* Binary I/O               */
    (void) memset(wrkbuff,0x00,alloc_size);      /* Clear to null's          */
  }                                              /*                          */
  else {                                         /* Ascii I/O                */
    (void) memset(wrkbuff,0x20,alloc_size);      /* Clear to blank's         */
  }                                              /*                          */

  if((func & 0x60) == 0x20) {                    /* Write operation          */
    (void) memcpy(wrkbuff,buffer,mov_bytes);     /* Move bytes for I/O       */
  }                                              /*                          */

  (*iobuff)  = wrkbuff;                          /* Return buffer address    */
  (*records) = nrecs;                            /* Number of record/sectors */

  exit :

#if 0
  dLog(DLOG_STDOUT,"alloc_iobuff : dcod=%3d, fc=%2X, iobuff=%8X",
          dcode,func,wrkbuff);
  dLog(DLOG_STDOUT,",alloc_size=%5d, recl=%5d, mov_bytes=%5d",
          alloc_size,reclen,mov_bytes);
#endif

  return(alloc_size);                            /* Size of the I/O operation*/
}


/*===========================================================================*/
/* rewind_file  - Perform a Rewind Operation on a file                       */
/*===========================================================================*/

void rewind_file_(const int *lu, int * status)
{
  iopcb      pblk;
  int        func;
  char       buffer[8];
  int        buflen;
  int        radr;

  func        = 0xC0;                       /* Set the rewind function code   */
  (void)      memcpy(buffer,"<REWIND>",8L); /* Make the buffer say <REWIND>   */
  buflen      = 8L;                         /* 8 bytes in length for buffer   */
  radr        = 0L;                         /* No Random address              */

  dskio(&pblk, &func, (int *)lu, buffer, &buflen, &radr);
  
  (*status)   = pblk.stat;

  return;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

