/************************************************************************************/
/*                                                                                  */
/* queue.c      FORTRAN Call Queue Routine                                R01-00.00 */
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
/*   No arguments                                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* ECHO_QUEUE_PARMS is referenced in queue.c and enable.c to allow for debug of the */
/* queue parameter data sent between task.  The routine dbg_queue_write_ is called  */
/* from gb1_init.f to enable the debugging.                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/*            DRLJr        11/Apr/1997  Handle case where the taskname passed is    */
/*                                      zeros which indicate a self directed call.  */
/*            DRLJr        12/Dec/1997  Removed unneeded structure and added        */
/*                                      documentation.                              */
/*            DRLJr        31/Mar/1998  Corrected null termination of local copy of */
/*                                      taskname.  Code was changing actual argument*/
/*                                      instead of local copy.                      */
/*            DRLJr        30/Mar/1999  Moved the ECHO_QUEUE_PARMS and access and   */
/*                                      setting rooutines to new file queue_dbg.c.  */
/*                                      This was done since being here would cause  */
/*                                      unneeded code to be pulled in at link time  */
/*                                      causing eroneous linking.                   */
/* R01-00.00  DRLJr        23/May/2000  Added standard documentation and addd code  */
/*                                      to support the sending of more than 4 bytes */
/*                                      with the queue parameter mechanism.  The    */
/*                                      current task queue support code in enable.c */
/*                                      currently supports the reception of 1024    */
/*                                      bytes of data in a single request.          */
// tt80093    gj           22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <dlogdefs.h>
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093


/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

extern int        ECHO_QUEUE_PARMS;

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

int    get_slot_();
int    get_mid_();
int    get_pid_();
int    getmqcount(int mqid);  //60634
int    kill();

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

#define      MSGSIZE      4

static char  mytaskname[12] = { 'n','o','t',' ','s','e','t',' ','\0','\0','\0','\0' };

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

static char error01[] =
 {" %-8.8s : QUEUE Bad task name %-8.8s, Item is %08X, Error %d \n"};
static char error04[] =
 {" %-8.8s : QUEUE Can not find task %-8.8s, Item is %08X \n"};


/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

void queue_n_bytes(char * taskname,int      * par1,int      * stat, int      nbytes)
{
  int    slot;                               /* Slot number                         */
  int    mid;                                /* Message Queue Id                    */
  int    pid;                                /* Process Id                          */
  int    ret;                                /* Working Return status value         */
  int    indx;                               /* Working Index variable              */
  int    flag;                               /* Working Flag                        */
  int    mcnt;                               /* count of pending msgs on queue 60634*/ 
  union {                                    /* Work Variable for taskname work to  */
    int      iname[ 3];                      /* detirmine if the taskname being     */
    char     tname[12];                      /* supplied is a binary zero or an     */
  } TskName;                                 /* actual taskname.                    */
  FILE* fd;

  /*--------------------------------------------------------------------------------*/
  /* Move the taskname to the internal data area and convert the lower case         */
  /* characters to uppercase characters.                                            */
  /*--------------------------------------------------------------------------------*/

  for(indx=0;indx<8;indx++) {                /* Move the taskname and make uppercase*/
    TskName.tname[indx] = (char) toupper((int) taskname[indx]); /* Convert byte     */
  }                                          /*                                     */
  TskName.iname[2] = 0;                      /* Insure we have null terminatation   */

  /*--------------------------------------------------------------------------------*/
  /* Detirmine if this a self-directed call or if the name contains a taskname.     */
  /*--------------------------------------------------------------------------------*/

  if(TskName.iname[0] == 0) {                /* Self Directed if 1st 4 bytes are 0  */
    get_tname_(TskName.tname,8L);            /* Get our Task Name for processing    */
  }                                          /*                                     */
  else {                                     /* Other Directed (may still ourself)  */
    flag = check_tname_(TskName.tname,8L);   /* Check task name (insure its valid)  */
    if(flag > 0) {                           /* Error in the task name ?            */
      (void) get_tname_(mytaskname,8L);      /* Get our taskname                    */
      fd = dLogTypes[DLOG_MAJOR].fd;
      dLog(DLOG_MAJOR,error01,mytaskname,TskName.tname,*par1,flag);/* Write message */
      fprintf(fd,"  Taskname supplied in Hex is : ");              /*               */
      for(indx=0;indx<8;indx++) {                                  /*               */
         fprintf(fd,"%02X ",TskName.tname[indx]);                  /*               */
      }                                                            /*               */
      fprintf(fd,"\n");                                            /*               */
      (*stat) = 1;                           /* Report a taskname error             */
      return;                                /* Return to the caller                */
    }                                        /*                                     */
  }                                          /*                                     */
  slot = abs(get_slot_(TskName.tname,8L));        /* Get the slot number for the task    */

  /*--------------------------------------------------------------------------------*/
  /* Detirmine if taskname was found in the tables                                  */
  /*--------------------------------------------------------------------------------*/

  if (slot == 0) {                          /* Task specified by taskname not found */
    (void) get_tname_(mytaskname,8L);       /* Get our taskname for error message   */
    dLog(DLOG_WARNING,error04,mytaskname,TskName.tname,*par1); /* Write the message */
    *(stat) = 4;                            /* Set the status to 4                  */
    return;                                 /* Return to the caller                 */
  }                                         /*                                      */


  /*--------------------------------------------------------------------------------*/
  /* Get Message Queue Id and The Process Id for the receiver                       */
  /*--------------------------------------------------------------------------------*/

  mid = get_mid_(&slot);
  pid = get_pid_(&slot);
  mcnt = getmqcount(mid);                   //ttt 60634
  /*--------------------------------------------------------------------------------*/
  /* Send the information to the required tasks                                     */
  /*--------------------------------------------------------------------------------*/

  ret = send_trap(0x01,mid,pid,(char *)par1,nbytes);
  if (ret < 0) {
    (void) get_tname_(mytaskname,8L);
    if(ret == -1L) {
      dLog(DLOG_MAJOR,"%-8.8s : QUEUE Can not send parameter to %-8.8s of %08X.(Pending=%d) \n",
          mytaskname, taskname, *par1, mcnt);
    }
    else {
      dLog(DLOG_MAJOR," %-8.8s : QUEUE Signal Transmission Err to %-8.8s Item=%08X Err=%d (Pending=%d)",
          mytaskname,taskname,*par1,ret, mcnt);
    }
    (*stat) = 12;
    return;
  }

  /*--------------------------------------------------------------------------------*/
  /* Report the message information if flag is enabled                              */
  /*--------------------------------------------------------------------------------*/

  (void) get_tname_(mytaskname,8L);
  dLog(DLOG_CALLQUEUE,
     " %-8.8s : QUEUE Queued Item sent to task %-8.8s, Item is %08X (Pending=%d) ",
     mytaskname,TskName.tname,*par1, mcnt);

  if (mcnt > 1000) { 
     dLog(DLOG_WARNING, 
     " %-8.8s : ABNORMAL # OF PENDING Queued Items to task %-8.8s, Item is %08X (Pending=%d)",
     mytaskname, TskName.tname,*par1, mcnt);
  }
  (*stat) = 0;            /* Fortran completion status */
  return;
}


/*==================================================================================*/
/* Enhanced FORTRAN QUEUE() parameter calls                                         */
/*==================================================================================*/

void queue_(char * taskname,int      * par1,int      * stat)
{
  (void)   queue_n_bytes(taskname,par1,stat,4L);
  return;
}

/*==================================================================================*/
/* Enhanced FORTRAN QUEUE() parameter calls                                         */
/*==================================================================================*/

void queue_fullwords_(char * taskname,int      * par1,int      * size, int      * stat)
{
  int                   nbytes;

  nbytes = (*size) * 4L;
  (void)   queue_n_bytes(taskname,par1,stat,nbytes);
  return;
}

/*==================================================================================*/
/* Enhanced FORTRAN QUEUE() parameter calls                                         */
/*==================================================================================*/

void queue_halfwords_(char * taskname,int      * par1,int      * size, int      * stat)
{
  int                   nbytes;

  nbytes = (*size) * 2L;
  (void)   queue_n_bytes(taskname,par1,stat,nbytes);
  return;
}

/*==================================================================================*/
/* Enhanced FORTRAN QUEUE() parameter calls                                         */
/*==================================================================================*/

void queue_bytes_(char * taskname,int      * par1,int      * size, int      * stat)
{
  int                   nbytes;

  nbytes = (*size);
  (void)   queue_n_bytes(taskname,par1,stat,nbytes);
  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
