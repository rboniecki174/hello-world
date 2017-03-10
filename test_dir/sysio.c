/**********************************************************************/
/*                                                                    */
/* sysio.c                                                            */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "devicecodes.h"
#include "sysio.h"
#include "dlogdefs.h"

void get_lu_fd_(int      * Lu, int      * YesNo, int      * UnixFd);
void get_lu_dcod_(int      * Lu, int      * Dcode);
int getfd_(int      * lu);
void dskio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                        int      * size, int      * radr);
void tcpio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                       int      * size, int      * radr);
void ttyio(iopcb * pcb, int      * func, int      * lu,   char     * buff,
                       int      * size, int      * radr);
int isatty(int desc);

#define  TCPIO   (void) tcpio(pcb,func,lu,buff,size,rand)
#define  TTYIO   (void) ttyio(pcb,func,lu,buff,size,rand)
#define  DSKIO   (void) dskio(pcb,func,lu,buff,size,rand)

#define  ISTTY   ((Dcode >= DCOD_BRKSHRT) && (Dcode <= DCOD_TTY))
#define  ISPRT   (Dcode == DCOD_PRINTER)

#define  WRITE(x,y,z) /*dLog(DLOG_MINOR,"I/O : %-8.8s %5.5s Lu=%3d\n",x,y,z)*/

#define  DEBUGG 0

#if DEBUGG

static void wrtinfo(iopcb * pcb, int      * func, int      * lu,
                                 char     * buff, int      * size,
                                 int      * rand, int     );
#endif

int         check_invoked_(void);

void sysio_(iopcb * pcb, int      * func, int      * lu,   char * buff,
                         int      * size, int      * rand              )
{
  int         IsTCP;                 /* Lu is a TCP connection       */
  char      * b_ead;                 /* End of buffer                */
  int         UnixFd;                /* Unix File Descriptor         */
  int         Dcode;                 /* Assigned Device Code         */
  int         IsTTY;                 /* Lu is a TTY connection       */
  int         invoked;               /* Check for invoked            */

  invoked    = check_invoked_();     /* Has process_asn been called  */
  if(invoked == 0) {                 /* No, report assign errors     */
    pcb->stat = 0x8100+((*lu)&0xFF); /* Indicate error               */
    return;                          /* Exit now                     */
  }                                  /*                              */

  b_ead      = (char *) ((int     ) buff  + (*size) - 1);

  pcb->fc    = (char) (*func);       /* Store the function code      */
  pcb->lu    = (char) (*lu);         /* Store the logical unit       */
  pcb->stat  = (short     ) 0L;       /* Clear the status             */
  pcb->sad   = (int)buff;                 /* Store the buffer address     */
  pcb->ead   = (int)b_ead;                /* Store the end address        */
  pcb->rad   = (*rand);              /* Random address               */
  pcb->lxf   = (int     ) 0L;        /* Clear length of transfer     */

//#if DEBUGG
//  if((*lu) == 1L) {
//    dLog(DLOG_MAJOR,"****************************************");
//    dLog(DLOG_MAJOR,"****************************************\n");
//    wrtinfo(pcb,func,lu,buff,size,rand);
//  }
//#endif

  if(((*lu) < 0) || ((*lu) > 255)) { /* Illegal Logical Unit         */
    pcb->stat = 0x8100;              /* Flag as illegal              */
    return;                          /* Return to caller             */
  }                                  /*                              */

  if((*func) == 0x08) {              /* Wait on I/O ?                */
    return;                          /* Just Return to caller        */
  }                                  /*                              */

  (void) get_lu_fd_(lu,&IsTCP,&UnixFd); /* Get assignment info       */
  (void) get_lu_dcod_(lu,&Dcode);    /* Get the device code          */


  if(UnixFd >= 0) {                  /* Assigned by process_asn      */
    if(IsTCP != 0) {                 /* LU is on a TCP connection    */
      WRITE("ENTER  A","TCPIO",*lu);
      TCPIO;                         /* Invoke the TCP I/O function  */
      WRITE("EXTT   A","TCPIO",*lu);
    }                                /*                              */
    else if(ISTTY) {                 /* This is a tty type device    */
      WRITE("ENTER  A","TTYIO",*lu);
      TTYIO;                         /* Invoke the TTY I/O function  */
      WRITE("EXIT   A","TTYIO",*lu);
    }                                /*                              */
//73049    else if(ISPRT) {                 /* This is a printer device     */
//73049      WRITE("ENTER  A","PRTIO",*lu);
//73049      PRTIO;
//73049      WRITE("EXIT   A","PRTIO",*lu);
//73049    }
    else {                           /* All else is Disk I/O         */
      WRITE("ENTER  A","DSKIO",*lu);
      DSKIO;                         /*                              */
      WRITE("EXIT   A","DSKIO",*lu);
    }                                /*                              */
  }                                  /*                              */
  else {                             /* Not assigned by process_asn  */
    IsTTY = isatty(getfd_(lu));     /* Is this a tty?               */
    if(IsTTY != 0) {                 /* Yes it is a tty              */
      WRITE("ENTER  B","TTYIO",*lu);
      TTYIO;                         /* Use TTY I/O processor        */
      WRITE("EXIT   B","TTYIO",*lu);
    }                                /*                              */
    else {                           /* Use DSKIO processor          */
      WRITE("ENTER  B","DSKIO",*lu);
      DSKIO;                         /* Use Disk I/O Processor       */
      WRITE("EXIT   B","DSKIO",*lu);
    }                                /*                              */
  }                                  /*                              */

#if DEBUGG
  if((*lu) == 1L) {
    wrtinfo(pcb,func,lu,buff,size,rand);
  }
#endif

  return;                            /*                              */
}                                    /*                              */

/*====================================================================*/
/* Write Debug Information                                            */
/*====================================================================*/

#if DEBUGG

static void wrtinfo(iopcb * pcb, int      * func, int      * lu,
                                 char     * buff, int      * size,
                                 int      * rand)
{
  int       index,jndex;

  dLog(DLOG_MAJOR,"========================================");
  dLog(DLOG_MAJOR,"========================================\n");
  dLog(DLOG_MAJOR,"A(PBLK) = %08X",pcb);
  dLog(DLOG_MAJOR,", ");
  dLog(DLOG_MAJOR,"FC =      %02X",(*func));
  dLog(DLOG_MAJOR,", ");
  dLog(DLOG_MAJOR,"LU =      %3d",(*lu));
  dLog(DLOG_MAJOR,", ");
  dLog(DLOG_MAJOR,"A(BUFFER) = %08X",buff);
  dLog(DLOG_MAJOR,", \n");
  dLog(DLOG_MAJOR,"SIZE    = %5d",(*size));
  dLog(DLOG_MAJOR,", ");
  dLog(DLOG_MAJOR,"RAND    = %5d",(*rand));
  dLog(DLOG_MAJOR,"\n");

  for(index=0;index<(*size);index+=16) {
    dLog(DLOG_MAJOR," %6d  ",index);
    for(jndex=index+0;jndex<index+8;jndex++) {
      dLog(DLOG_MAJOR," %02X",(buff[jndex]&0xFF));
    }
    dLog(DLOG_MAJOR,"  ");
    for(jndex=index+8;jndex<index+16;jndex++) {
      dLog(DLOG_MAJOR," %02X",(buff[jndex]&0xFF));
    }
    dLog(DLOG_MAJOR,"  * ");
    for(jndex=index;jndex<index+16;jndex++) {
      if((buff[jndex]<' ') || (buff[jndex]>'~')) {
        dLog(DLOG_MAJOR,".");
      }
      else {
        dLog(DLOG_MAJOR,"%1c",(buff[jndex]&0xFF));
      }
    }
    dLog(DLOG_MAJOR," * \n");
  }
  dLog(DLOG_MAJOR,"----------------------------------------");
  dLog(DLOG_MAJOR,"----------------------------------------\n");
  return;
}

#endif

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

