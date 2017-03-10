/************************************************************************************************/
/*                                                                                              */
/* ttyinit.c                     Support for CRTs                                     R00-01.00 */
/*                                                                                              */
/* ============================================================================================ */
/*                                                                                              */
/* Revision   Who      When         Description                                                 */
/* =========  =======  ===========  =========================================================== */
/* R00-00.00  DRLJr    18/Jun/1997  Created from crtio.c                                        */
/* R00-01.00  DRLJr    07/Apr/1999  Changed references of getfd_ to GetFD_ so                   */
/*                                  code can be used in both "C" and FORTRAN.                   */
/*                                  GetFD_() is located in process_asnc.c and                   */
/*                                  process_asnf.c.  The file process_asnc.c                    */
/*                                  is used for "C" main programs and the file                  */
/*                                  process_asnf.c is used for FORTRAN main                     */
/*                                  programs.  For FORTRAN main programs the                    */
/*                                  routine GetFD_() calls the FORTRAN RTL                      */
/*                                  routine getfd_() and checks the shared                      */
/*                                  memory segments reatime1 if getfd_()                        */
/*                                  returns a -1 value.  For the "C" main                       */
/*                                  programs the shared memory segment is used                  */
/*                                  to detimine the unix file descriptor based                  */
/*                                  on the supplied logical unit.                               */
// tt80093    gj       22/May/2014  Clean up prototype
/*                                                                                              */
/************************************************************************************************/

#include <stdio.h>
#include <sched.h>
#include <termio.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include "devicecodes.h"
#include "sysio.h"
#include "dlogdefs.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

static char  taskname[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',0,0,0,0 };
static char  error1[]     = {"%8.8s : **ERROR** LU %3d is not OPEN'ed (connected)!        "};
static char  error2[]     = {"%8.8s : *WARNING* LU %3d is not attached to a TTY port.     "};
static char  error3[]     = {"%8.8s : **ERROR** LU %3d unable to get current tty settings."};
static char  error4[]     = {"%8.8s : **ERROR** LU %3d unable to set new tty settings.    "};

#define TASKNAME() get_tname_(taskname,8L)

#define DEFAULT_CFLAG     (CREAD | CLOCAL | CS8 )
#define DEFAULT_IFLAG     (IGNBRK | IXON | ICRNL) 

/*==============================================================================================*/
/* Common Terminal Initialization Processing                                                    */
/*==============================================================================================*/

int tty_init(int      * lu,int      * flg, int      type)
{
 struct termio tsetup;                      /* Terminal setup block        */
 int           fd;                          /* File Descriptor             */
 int           status;                      /* Working Status              */

 fd   = GetFD_(lu);                         /* Get the Unix fd        */
 if(fd == -1) {                             /* LU is not assigned     */
   TASKNAME();                              /* Get out taskname       */
   dLog(DLOG_MAJOR,error1,taskname,*lu);     /* Generate a error msg   */
   return(-1L);                             /* Tell caller of error   */
 }                                          /*                        */

 /* Set it up; this is old-fashioned (non-POSIX) tty stuff */

 status = ioctl(fd,TCGETA,&tsetup);         /* Get old terminal setup */
 if(status == -1) {                         /* IOCTL error            */
   TASKNAME();                              /* Get our taskname       */
   if(errno == ENOTTY) {                    /* Not a tty device       */
     dLog(DLOG_MINOR,error2,taskname,*lu);   /* Generate a warning     */
     return(0L);                            /* Treat as okay          */
   }                                        /*                        */
   dLog(DLOG_MAJOR,error3,taskname,*lu);     /* Report the problem     */
   return(-1L);                             /* Report as an error     */
 }                                          /*                        */

 /* Set up the common fields used by all of the terminal environments */

 tsetup.c_cc[VMIN]   =  1;                  /* Minimum number of      */
                                            /* characters to get      */
 tsetup.c_cc[VTIME]  =  0;                  /* Timeout in 100 mSec;   */
                                            /* 0 = wait until we get  */
                                            /* VMIN characters        */
 tsetup.c_cc[VKILL]  = '\0';                /* Disable the kill       */
                                            /* character.             */
 tsetup.c_cc[VERASE] = '\b';                /* Set backspace to ^H    */

 /* Set up the c_iflag field so that the terminal will use a CR as the*/
 /* EOL character for terminals.                                      */

 tsetup.c_iflag      = DEFAULT_IFLAG;       /* XON and CR = CR        */

 /* Set up the c_lflag field based upon the particular device type    */
 /* For all terminal devices we will enable ECHOing, Backspace will   */
 /* erase the character with a BS-SP-BS and a line erase will use a   */
 /* series of BS-SP-BS sequences.                                     */

 tsetup.c_lflag      = ICANON | ECHO | ECHOE | ECHOKE;

 /* Set up the c_cflag based upon the device.  The only difference is */
 /* that of the specific line speed.                                  */

 tsetup.c_cflag      = DEFAULT_CFLAG;       /* Set the default part   */
 switch(type) {                             /* Set the speed          */
   case DCOD_BRKSHRT :                      /* BRKSHRT Terminal       */
   case DCOD_SYSEXC  :                      /* SYSEXC Terminal        */
   case DCOD_TTY     :                      /* TTY Port               */
          tsetup.c_cflag |= B19200;         /* Set up as 19200        */
          break;                            /*                        */
   case DCOD_MARPAD  :                      /* MDEQTRNS Port          */
/*        tsetup.c_iflag  &=  ~(IXON | IXOFF);*/  /* Disable XON/XOFF       */
          tsetup.c_iflag  = IGNBRK;         /* Disable XON/XOFF       */
          tsetup.c_cflag |= B9600;          /* Set up as 9600         */
          tsetup.c_oflag  &=  ~(OPOST);     /* Disable Post processing*/
          tsetup.c_lflag  &=  ~(ICANON | ECHO | ECHOE | ECHOKE);    /* No canonicle processing*/
          tsetup.c_cc[VMIN] = 100;
          tsetup.c_cc[VTIME] = 1;

          break;                            /*                        */
   case DCOD_PRINTER :                      /* PRINTER on a /dev/tty? */
          tsetup.c_cflag |= B9600;          /* Set up as 9600         */
          break;                            /*                        */
   default           :                      /* Generic tty device     */
          tsetup.c_cflag |= B19200;         /* Set up as 19200        */
          break;                            /*                        */
 }                                          /*                        */

 status = ioctl(fd,TCSETA,&tsetup);         /* New terminal setup     */
 if(status == -1) {                         /* IOCTL error            */
   TASKNAME();                              /* Get our taskname       */
   dLog(DLOG_MAJOR,error4,taskname,*lu);     /* Report the problem     */
   return(-1L);                             /* Report as an error     */
 }                                          /*                        */

 return(status);                            /* Return final status    */

}                                           /* tty_init_comn          */

/*==============================================================================================*/
/* SYSEXC Terminal Initialization Processing                                                    */
/*==============================================================================================*/

int ttys_init_(int      * lu,int      * flg)
{
 int          status;                       /* Return status variable */

 status = tty_init(lu,flg,DCOD_SYSEXC);     /* Call common routine    */

 return(status);                            /* Return with the status */

}                                           /* ttys_init_             */

/*==============================================================================================*/
/* BRKSHRT Terminal Initialization Processing                                                   */
/*==============================================================================================*/

int ttyb_init_(int      * lu,int      * flg)
{
 int          status;                       /* Return status variable */

 status = tty_init(lu,flg,DCOD_BRKSHRT);    /* Call common routine    */

 return(status);                            /* Return with the status */

}                                           /* ttyb_init_             */

/*==============================================================================================*/
/* MARPAD Terminal Initialization Processing                                                    */
/*==============================================================================================*/

int ttym_init_(int      * lu,int      * flg)
{
 int          status;                       /* Return status variable */

 status = tty_init(lu,flg,DCOD_MARPAD);     /* Call common routine    */

 return(status);                            /* Return with the status */

}                                           /* ttym_init_             */

/*==============================================================================================*/
/* tty Terminal Initialization Processing                                                       */
/*==============================================================================================*/

int ttyt_init_(int      * lu,int      * flg)
{
 int          status;                       /* Return status variable */

 status = tty_init(lu,flg,DCOD_TTY);        /* Call common routine    */

 return(status);                            /* Return with the status */

}                                           /* ttym_init_             */

/*==============================================================================================*/
/* PRINTER Terminal Initialization Processing                                                   */
/*==============================================================================================*/

int ttyp_init_(int      * lu,int      * flg)
{
 int          status;                       /* Return status variable */

 status = tty_init(lu,flg,DCOD_PRINTER);    /* Call common routine    */

 return(status);                            /* Return with the status */

}                                           /* ttyp_init_             */

/*==============================================================================================*/
/* Set Terminal for Image or Binary I/O                                                         */
/*==============================================================================================*/

int tty_image_binary(int      lu, int      fc, int      fd)
{
 int           status;                      /* Return status variable */
 struct termio tsetup;                      /* Terminal setup block   */

 status = ioctl(fd,TCGETA,&tsetup);         /* Get old terminal setup */
 if(status == -1) {                         /* IOCTL error            */
   TASKNAME();                              /* Get our taskname       */
   if(errno == ENOTTY) {                    /* Not a tty device       */
     dLog(DLOG_MINOR,error2,taskname,lu);    /* Generate a warning     */
     return(0L);                            /* Treat as okay          */
   }                                        /*                        */
   dLog(DLOG_MAJOR,error3,taskname,lu);      /* Report the problem     */
   return(-1L);                             /* Report as an error     */
 }                                          /*                        */

 if((fc & 0x11) == 0x11) {                  /* Binary/Image I/O       */
   tsetup.c_iflag  &=  ~(IXON | IXOFF);     /* Disable XON/XOFF       */
   tsetup.c_oflag  &=  ~(OPOST);            /* Disable Post processing*/
   tsetup.c_lflag  &=  ~(ICANON);           /* No canonicle processing*/
 }                                          /*                        */
 else if((fc & 0x10) == 0x10) {             /* Binary I/O selected    */
   tsetup.c_iflag  &=  ~(IXON | IXOFF);     /* Disable XON/XOFF       */
   tsetup.c_oflag  &=  ~(OPOST);            /* Disable Post processing*/
   tsetup.c_lflag  &=  ~(ICANON);           /* No canonicle processing*/
 }                                          /*                        */
 else if((fc & 0x01) == 0x01) {             /* Image I/O selected     */
   tsetup.c_iflag  &=  ~(IXON | IXOFF);     /* Disable XON/XOFF       */
   tsetup.c_oflag  &=  ~(OPOST);            /* Disable Post processing*/
   tsetup.c_lflag  &=  ~(ICANON);           /* No canonicle processing*/
 }                                          /*                        */
 else {                                     /* Re-enable processing   */
   tsetup.c_iflag  |=   (IXON | IXOFF);      /* Disable XON/XOFF       */
   tsetup.c_oflag  |=   (OPOST);             /* Post processing        */
   tsetup.c_lflag  |=   (ICANON);            /* Canonicle processing   */
 }                                          /*                        */

 status = ioctl(fd,TCSETA,&tsetup);         /* New terminal setup     */
 if(status == -1) {                         /* IOCTL error            */
   TASKNAME();                              /* Get our taskname       */
   dLog(DLOG_MAJOR,error4,taskname,lu);      /* Report the problem     */
   return(-1L);                             /* Report as an error     */
 }                                          /*                        */

 return(status);                            /* Return with status     */
}                                           /* tty_image_binary       */

/***********************************************************************************************/
/*                                                                                             */
/***********************************************************************************************/

