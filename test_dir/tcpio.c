/************************************************************************************************/
/*                                                                                              */
/* tcpio.c                       Support for CRT on TCP Connections                   R01-02.00 */
/*                                                                                              */
/*==============================================================================================*/
/*                                                                                              */
/* Programming Notes                                                                            */
/*                                                                                              */
/*                                                                                              */
/*==============================================================================================*/
/*                                                                                              */
/* Revision History                                                                             */
/*                                                                                              */
/* Revision   When         Who       Why                                                        */
/* =========  ===========  ========  ========================================================== */
/* R01-01.00  02/Oct/1998  DRLJr     IF EOF is reported from a socket generate an fatal error   */
/* R01-02.00  07/Apr/1999  DRLJr     Changed references of getfd_ to GetFD_ so                  */
/*                                   code can be used in both "C" and FORTRAN.                  */
/*                                   GetFD_() is located in process_asnc.c and                  */
/*                                   process_asnf.c.  The file process_asnc.c                   */
/*                                   is used for "C" main programs and the file                 */
/*                                   process_asnf.c is used for FORTRAN main                    */
/*                                   programs.  For FORTRAN main programs the                   */
/*                                   routine GetFD_() calls the FORTRAN RTL                     */
/*                                   routine getfd_() and checks the shared                     */
/*                                   memory segments reatime1 if getfd_()                       */
/*                                   returns a -1 value.  For the "C" main                      */
/*                                   programs the shared memory segment is used                 */
/*                                   to detimine the unix file descriptor based                 */
/*                                   on the supplied logical unit.                              */
/*                                                                                              */
/*           07/19/2007  jl          reconnect after sigpipe                                    */ 
// tt80093   06/17/2014  gj          Clean up prototypes
/************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <termio.h>
#include <fcntl.h>
#include <time.h>
#define _XOPEN_SOURCE 500           //required by linux for sigset prototype
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <dlogdefs.h>
#include "devicecodes.h"
#include "process_asn.h"
#include "private_data.h"
#include "process_proto.h"
#include "process_asnc.c"

#include "sysio.h"

#define ACK 0x06


extern void (*enable_handler[])();             /* registered handlers */ 

static int tcpread(int fc, int fd, char * buffer, int size,                         //tt80093
		   int * bytesread, int dev_code);                                  //tt80093

//void dumpbuf_condensed_(int *idx, unsigned char *obuff, char* str, char* fncstr); //tt80093
//void check_if_tcp_(int      * Lu, int      * YesNo, int      * UnixFd);           //tt80093
//void get_lu_dcod_(int      * Lu, int      * Dcode);                               //tt80093
//int      self_trap(int      rc, char * buffer, int      size);                    //tt80093
//void monitor_rd_(int      * UnixFd, char      * bytes,     int      * length,     //tt80093
//               int      * status, int         bytes_len);                         //tt80093
//int      monitor_rd(int      UnixFd, char * bytes, int      length);              //tt80093
//void gb_exit_(int      * eot);                                                    //tt80093
//void gb_exit(int      eot);                                                       //tt80093
//int  tty_image_binary(int      lu, int      fc, int      fd);                     //tt80093
//int  self_trap(int rc, char * buffer, int      size);                             //tt80093
//void (*sigset (int sig, void (*disp)(int)))(int);    //tt80093 note: proto for linux taken from solaris
//int sigrelse(int sig);                               //tt80093 note: proto for linux taken from solaris


static char   ranerr[] = {"%8.8s : *WARNING* : Random I/O to tcp ignored (FC=x'%02X', LU=%3d)"};
//static char   prcerr[] = {"%8.8s : *WARNING* : Proceed I/O to tcp ignored (FC=x'%02X', LU=%3d)"};
static char   tserr[]  = {"%8.8s : **ERROR** : Test and Set I/O to tcp (FC=x'%02X', LU=%3d)"};
static char   ufcerr[] = {"%8.8s : **ERROR** : Unknown Function Code (tcp) (FC=x'%02X', LU=%3d)"};
static char   rderr[]  = {"%8.8s : **ERROR** : Read from tcp failed, lu = %4d, errno = %4d" };
static char   wrerr[]  = {"%8.8s : **ERROR** : Write to tcp failed, lu = %4d, errno = %4d" };
//static char   ackerr[] = {"%8.8s : *WARNING* : Read from tcp with ack's, lu = %4d" };
static char   taskname[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',0,0,0,0 };
//static char   ioinfo[] = {"%8.8s : func : %02.2X, lu : %3.3, Buffer : %8X - %8X" };

static char   bs_erase[3]  = { 0x08, 0x20, 0x08 };
static char   cr_lf[2]     = { 0x0D, 0x0A       };

#define TASKNAME() get_tname_(taskname,8L)
void sig_action(int sigs);  // jl 20070719
/*==============================================================================================*/
/*                                                                                              */
/*==============================================================================================*/

void tcpio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                        int      * size, int      * radr, int      * xopt)
{ 
 int              status;                        /* General Status Variable      */
 int              dev_code;                      /* Associated Device Code       */
 int              UnixFd;                        /* Associated unix fd           */
 int              Lu;                            /* Local copy of logical unit   */
 int              YesNo;                         /* TCP IP Flag                  */
 int              BytesRead;                     /* Number bytes read from tty   */
 int              BytesWrite;                    /* Number bytes written to tty  */
 char           * Buffer;                        /* Buffer for I/O               */
 int              offset;                        /* Working read offset          */

 Lu      = *lu;                                  /* Copy the LU         */
 Buffer  = buff;                                 /* Move buffer address */

 (void) check_if_tcp_(&Lu,&YesNo,&UnixFd);       /* Check if tcp        */
 (void) get_lu_dcod_ (&Lu,&dev_code);            /* Get the device code */
 if(UnixFd < 0) {                                /* Openned by FORTRAN  */
   UnixFd = GetFD_(lu);                          /* Get the UNIX fd     */
 }                                               /*                     */

/* dLog(DLOG_STDLOG,ioinfo,func,lu,pcb->sad,pcb->ead);
*/
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

#if 0
 if(((*func) & 0x11) != 0x00) {                  /* Binary/Image ??     */
   status = tty_image_binary(Lu,(*funcc),UnixFd);/* Make tty change     */
 }                                               /*                     */
#endif
TASKNAME();
if (strncmp(taskname, "SCNTRA", 6L) == 0) {
   sigset (SIGPIPE, sig_action);
}
 if(((*func) & 0xE0) == 0x40) {                  /* Read from tty       */
   offset    = 0;                                /* Clear buffer offset */
   BytesRead = 0;                                /* Clear counter       */
   for(;;) {                                     /* Read until were done*/
     status = tcpread((*func),UnixFd,Buffer,(*size),&BytesRead,dev_code);/* Read */
     if(BytesRead == (*size)) {                  /* Buffer is full      */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesRead;                  /* Set number read     */
       break;                                    /*                     */
     }                                           /*                     */
     else if(status < 0) {                       /* Error reported      */
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
#if 0
     dLog(DLOG_MINOR,"\n");
     c = BytesRead;
     c = ((c +  7) /  8) *  8;
     for(b = 0; b < c; b +=  8) {
       for(a = b; a < b +  8 ; a++) {
         dLog(DLOG_MINOR,"B[%3d]=%02X ",a, (buff[a] & 0xFF));
       }
       dLog(DLOG_MINOR,"\n");
     }
#endif
#if 0
     for(a = 0; a < BytesRead; a++) {            /* Look for an non-ack */
       dLog(DLOG_MINOR"BUFF[%3d]=%02X\n",a, (buff[a] & 0xFF));
     }                                           /*                     */
#endif
   }                                             /*                     */
 }                                               /* End of the read     */
 else if(((*func) & 0xE0) == 0x20) {             /* Write to tty        */
   for(;;) {                                     /* Read until were done*/
     BytesWrite = write(UnixFd,Buffer,(*size));  /* Write the bytes     */
     tcdrain(UnixFd);
     if(BytesWrite == (*size)) {                 /* Buffer is written   */
       pcb->stat  =  0x0000;                     /* Set good status     */
       pcb->lxf   =  BytesWrite;                 /* Set number written  */
       break;                                    /*                     */
     }                                           /*                     */
     else if (BytesWrite < 0) {                   /* Error reported      */
       if (errno != EINTR) {                      /* Not a signal intr.  */
          pcb->stat = 0x8000 | errno;             /* Give an error status*/
          pcb->lxf  = 0;                          /* No data moved       */
          dumpbuf_condensed_((int *)size,(unsigned char *)Buffer,"Write Buffer SIGPIPE","TCPIO");
          status = DettachLU_Language(UnixFd);
          millisleep(5);
          status = AttachLU_TCP();
          millisleep(10);
          TASKNAME();                             /* Get the taskname    */
           dLog(DLOG_MAJOR,wrerr,taskname,Lu,errno);/* Report the error    */
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

#if 0
 if(((*func)&0x11) != 0x00) {                    /* Binary/Image ??     */
   status = tty_image_binary(Lu,0x00,UnixFd);    /* Make tty change     */
 }                                               /*                     */
#endif

 return;                                         /* Return to caller    */
}                                                /* ttyio               */


/*==============================================================================================*/
/* tcpread support function                                                                     */
/*==============================================================================================*/

static int      tcpread(int      func,     int        UnixFd, char * buffer,  //tt80093
                 int      bufrsize, int      * offset, int      dev_code )
{
  char      byte;                                /* Byte that was read */
  int       status;                              /* Status word        */

  status    = 0;                                 /* Set to 0           */

  if((func & 0x11) == 0x00) {                    /* Formatted Read     */
    for(;;) {                                    /* Read until done    */
      if (dev_code == DCOD_BRKSHRT)
       {
          status = monitor_rd(UnixFd,&byte,1L);  /* Read one byte      */
          /* Note: monitor_rd changes a 0 status to -1 for a EOF error */
       }
      else
       {
          status = read(UnixFd,&byte,1L);        /* Read one byte      */
          if (status <= 0)
           {
              dLog(DLOG_MINOR,"STATUS=%d errno=%d\n",status,errno);
              perror("tcpread ERROR on READ:");
              gb_exit(254);
           }
       }
      if(status <  0){                           /* We have an error   */
        gb_exit(254);                            /* TCP connection lost*/
        break;                                                           
      }                                          /*                    */
      if(status == 1) {                          /* We have a character*/
        if(byte >= 0x20) {                       /* Printable character*/
          buffer[(*offset)] = byte;              /* Store the byte     */
          (*offset)++;                           /* Increment offset   */
          status = write(UnixFd,&byte,1L);       /* Echo the byte      */
        }                                        /*                    */
        else if(byte == 0x0D) {                  /* Read termination   */
          buffer[*(offset)] = byte;              /* Store the CR       */
          (*offset)++;                           /* Include the byte   */
          status = write(UnixFd,&cr_lf,2L);      /* Echo the byte      */
          status  = 0;                           /* Clear status       */
          break;                                 /* We are done        */
        }                                        /*                    */
        else if(byte == 0x08) {                  /* Backspace request  */
          if((*offset) > 0L) {                   /* 1+ chars in buffer */
            status = write(UnixFd,&bs_erase,3);  /* Write sequence     */
            (*offset)--;                         /* Backup one char.   */
          }                                      /*                    */
        }                                        /*                    */
        else if(byte == 0x1B) {                  /* Escape Character   */
          buffer[*(offset)] = byte;              /* Store the ESC      */
          (*offset)++;                           /* Update the offset  */
        }                                        /*                    */
        else {                                   /* All other chars    */
        }                                        /*                    */
      }                                          /*                    */
      if((*offset) >= bufrsize) {                /* Buffer is filled   */
        status = 0;                              /* Clear status       */
        break;                                   /* Leave loop         */
      }                                          /*                    */
    }                                            /*                    */
  }                                              /*                    */
  else if((func & 0x10) == 0x10) {               /* Binary Read        */
    for(;;) {                                    /* Read until done    */
      if (dev_code == DCOD_BRKSHRT)
       {
          status = monitor_rd(UnixFd,&byte,1L);  /* Read one byte      */
       }
      else
       {
          status = read(UnixFd,&byte,1L);        /* Read one byte      */
       }
      if(status == 0) {                          /* EOF error detected */
        status = -1;                             /* Indicate an error  */
      }                                          /*                    */
      if(status < 0) {                           /* We have an error   */
        gb_exit(254);                            /* TCP connection lost*/
        break;
      }                                          /*                    */
      if(status == 1) {                          /* We have a character*/
        buffer[(*offset)] = byte;                /* Store the byte     */
        (*offset)++;                             /* Increment the count*/
      }                                          /*                    */
      if((*offset) >= bufrsize) {                /* Buffer is filled   */
        status = 0;                              /* Clear status       */
        break;                                   /* Leave loop         */
      }                                          /*                    */
    }                                            /*                    */
  }                                              /*                    */
  else if((func & 0x11) == 0x01) {               /* Image Read         */
    for(;;) {                                    /* Read until done    */
      if (dev_code == DCOD_BRKSHRT)
       {
          status = monitor_rd(UnixFd,&byte,1L);  /* Read one byte      */
       }
      else
       {
          status = read(UnixFd,&byte,1L);        /* Read one byte      */
       }
      if(status == 0) {                          /* EOF error detected */
        status = -1;                             /* Indicate an error  */
      }                                          /*                    */
      if(status < 0) {                           /* We have an error   */
        gb_exit(254);                            /* TCP connection lost*/
        break;                                   /* Exit now           */
      }                                          /*                    */
      if(status == 1) {                          /* We have a character*/
        buffer[(*offset)] = byte;                /* Store the byte     */
        (*offset)++;                             /* Increment offset   */
        if(byte == 0x0D) {                       /* Read termination   */
          status  = 0;                           /* Clear status       */
          break;                                 /* We are done        */
        }                                        /*                    */
        status = write(UnixFd,&byte,1L);         /* Echo the byte      */
      }                                          /*                    */
      if((*offset) >= bufrsize) {                /* Buffer is filled   */
        status = 0;                              /* Clear status       */
        break;                                   /* Leave loop         */
      }                                          /*                    */
    }                                            /*                    */
  }                                              /*                    */
  return(status);                                /* Return the status  */
}

void sig_action(int SIG) {
   switch (SIG) {
      case SIGPIPE:
          dLog(DLOG_MINOR, "\n\ttcpio.c : SIGPIPE\n");
         sigrelse(SIGPIPE);
         break;
      default:
          dLog(DLOG_MINOR, "\n\ttcpio.c : default SIG [%d]\n", SIG);
         break;
   }
   return;
}
/***********************************************************************************************/
/*                                                                                             */
/***********************************************************************************************/

