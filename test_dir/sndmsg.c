/************************************************************************************/
/*                                                                                  */
/* sndmsg.c        FORTRAN Call SNDMSG Rotuine                            R01-00.00 */
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
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00               22/Apr/1997  File created                                */
/* R00-01.00  DRLJr        08/Dec/1997  Made needed corrections to allow the code   */
/*                                      to actually work.                           */
/* R01-00.00  DRLJr        20/Oct/1998  Fixed the call to get_slot_()  to pass the  */
/*                                      string length and added new tracking header.*/
/*                                      Also other changes to bring closer to       */
/*                                      standard code configuration.                */
// tt80093    gj           22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/signal.h>
#include "dlogdefs.h"
#include "sndmsg.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define   SNDMSG_TASKNAME    8L
#define   SNDMSG_DATA       64L

#define MSGSIZE  (SNDMSG_TASKNAME + SNDMSG_DATA)
#define MSGDATA  (                  SNDMSG_DATA)

/*==================================================================================*/
/*                                                                                  */
/* Note: ECHO_QUEUE_PARMS is referenced in queue.c and enable.c to allow for        */
/*       debug of the queue parameter data sent between task.  The routine          */
/*       dbg_queue_write_  is called from gb1_init.f to enable the debugging.       */
/*       Also now sndmsg.c                                                          */
/*                                                                                  */
/*==================================================================================*/

extern int        ECHO_QUEUE_PARMS;

static char       mytaskname[12] =
       { 'n','o','t',' ','s','e','t',' ','\0','\0','\0','\0' };

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

static char error01[] =
 {" %-8.8s : SNDMSG (status=1) Bad task name %-8.8s, Message can not be sent"};
static char error04[] =
 {" %-8.8s : SNDMSG (status=4) Can not find task %-8.8s int the realtime table. Message can not be sent."};
static char error08[] =
 {" %-8.8s : SNDMSG (status=8) Can not send message to %-8.8s, Message is misaligned (%08X)"};
//static char error09[] =
// {" %-8.8s : SNDMSG (status=9) Can not send message to %-8.8s, Not privileged"};
static char error12[] =
 {" %-8.8s : SNDMSG (status=12) Can not send message to %-8.8s"};
static char error12s[]=
 {" %-8.8s : SNDMSG (status=12) Signal Transmission Error to %-8.8s, Error %d"};
//static char error00[] =
// {" %-8.8s : SNDMSG ( 0) Message has been sent to task %-8.8s"};

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

void sndmsg_(char * taskname,char * buffer, int * stat)
{
  int slot, mid,pid, ret,indx,flag;
  int get_slot_(), get_mid_(), get_pid_();
  int kill();
  char * cpntr;

  struct my_msg {
    unsigned : 0;
    char sender_name[8];
    char message_data[MSGDATA];
  } my_msg;

  union {                                    /* Work Variable  */
    char     tname[12];                      /* for the OS/32  */
    int      iname[ 3];                      /* task name work */
  } TskName;                                 /*                */

/* Move taskname to the internal data area and convert lower   */
/* case characters to uppercase characters                     */

   for(indx=0;indx<8;indx++) {
     TskName.tname[indx] = (char) toupper((int) taskname[indx]);
   }
   TskName.iname[2] = 0;

/* Determine if this is a self directed call or is a call      */
/* directed to another task.                                   */

   if(TskName.iname[0] == 0) {               /* Self Directed  */
     get_tname_(TskName.tname,8L);           /* Get Task Name  */
     slot = abs(get_slot_(TskName.tname,8L));        /* Get the slot   */
   }                                         /*                */
   else {                                    /* Other Directed */
     flag = check_tname_(TskName.tname,8L);  /* Check task name*/
     if(flag > 0) {                          /* Error in name  */
       (void) get_tname_(mytaskname,8L);
       dLog(DLOG_MAJOR,error01,mytaskname,TskName.tname);
       *stat = 1;                            /* Name error     */
       return;                               /* Return         */
     }                                       /*                */
     slot = abs(get_slot_(TskName.tname,8L));     /* Get the slot   */
   }                                         /*                */

/* Determine if the taskname was located in the tables         */

   if (slot == 0) {                          /* Task not found */
     (void) get_tname_(mytaskname,8L);
     dLog(DLOG_MAJOR,error04,mytaskname,TskName.tname);
     *stat = 4;
     return;
   }

/* Check if the message address to send is fullword aligned    */

   if(((int     )buffer & 0x03) != 0x00) {
     (void) get_tname_(mytaskname,8L);
     dLog(DLOG_MAJOR,error08,mytaskname,TskName.tname,buffer);
     *stat = 8;
     return;
   }

/* Next, figure out QID & PID from Slot number */
   mid = get_mid_(&slot);
   pid = get_pid_(&slot);

/* Put something on the message queue */

   (void) get_tname_(my_msg.sender_name,8L);
   (void) memcpy(&my_msg.message_data,buffer,MSGDATA);
   cpntr = &my_msg.sender_name[0];

/* ret = send_trap(0x06,mid,pid,buffer,MSGSIZE);*/
   ret = send_trap(0x06,mid,pid,cpntr,MSGSIZE);
   if (ret < 0) {
     (void) get_tname_(mytaskname,8L);
     if(ret == -1L) {
       dLog(DLOG_MAJOR,error12,mytaskname, taskname);
     }
     else {
       dLog(DLOG_MAJOR,error12s,mytaskname,taskname,ret);
     }
     *stat = 12;
     return;
   }


   if (1) 
   {
     (void) get_tname_(mytaskname,8L);
     dLog(DLOG_STDLOG," %-8.8s : SNDMSG (Status=0) Message has been sent to task %-8.8s",
             mytaskname, TskName.tname);
     dLog(DLOG_STDLOG,"The first four bytes are : %x %x %x %x\n",
             buffer[0],buffer[1],buffer[2],buffer[3]);
   }

   (*stat) = 0;            /* Fortran completion status */
   return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/

