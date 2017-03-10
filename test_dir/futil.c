/************************************************************************************/
/*                                                                                  */
/* futil.c      FORTRAN Support Utilities                                 R00-01.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains "C" definition of the routines that were located in the file  */
/* futil.f.                                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   See the individual routines                                                    */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*  These routines are intended to be called from FORTRAN code or "C" code that     */
/*  acts like FORTRAN code.                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        25/Mar/1998  File created to replace futil.f             */
/* R00-01.00  DRLJr        30/Mar/1999  check_tname_() moved to its own file.       */
/* R00-01.01  DRLJr        06/Apr/1999  externs commented out.  extern definitions  */
/*                                      are located in the include files.           */
/* R01-00.00 jeffsun       21/Feb/2001  Replace realtime1 with REALTIME1 for sparc  */
/*                                      Clear of shared_lock with test_n_reset call */
// tt60135    GJ           02/Mar/2011  Replace obsolete realtime1.h with gb1_init.h
// tt80093    gj           05/May/2014  Clean up prototypes
// tt82513    gj           12/Nov/2014  Change test_and_reset/test_and_reset_ to
//                                      return void
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "gb1_init.h"
#include "private_data.h"
//#include "libos32.h"    //tt80093
#include "dlogdefs.h"
#include "proto_common.h" //tt80093

#if 0
extern   cmn_realtime1     realtime1;            /* Reference to the common block   */
extern   cmn_private_data  private_data;         /* Reference to the common block   */
#endif



/*==================================================================================*/
/*                                                                                  */
/* get_msgid()                                                                      */
/*                                                                                  */
/*==================================================================================*/

int      get_msgid_(void)
{
  int         slot;                              /* Slot Number in realtime common  */
  int         mqid;                              /* The extracted message queue id  */

  slot      = private_data.my_slot - 1L;         /* Get slot number (Make 0 based)  */
  mqid      = REALTIME1.shared_mqid[slot];       /* Get the message queue id        */

  return(mqid);                                  /* Return queue id to caller       */
}                                                /*                                 */

/*==================================================================================*/
/*                                                                                  */
/* get_mid()                                                                        */
/*                                                                                  */
/*==================================================================================*/

int      get_mid_(int      * ftnslot)
{
  int         slot;                              /* Slot Number in realtime common  */
  int         mqid;                              /* The extracted message queue id  */

  slot      = (*ftnslot) - 1L;                   /* Get slot number (Make 0 based)  */
  mqid      = REALTIME1.shared_mqid[slot];       /* Get the message queue id        */

  return(mqid);                                  /* Return queue id to caller       */
}                                                /*                                 */

/*==================================================================================*/
/*                                                                                  */
/* get_pid()                                                                        */
/*                                                                                  */
/*==================================================================================*/

int      get_pid_(int      * ftnslot)
{
  int         slot;                              /* Slot Number in realtime common  */
  int         pid;                               /* The extracted process id        */

  slot      = (*ftnslot) - 1L;                   /* Get slot number (Make 0 based)  */
  pid       = REALTIME1.shared_pid[slot];        /* Get the process id              */

  return(pid);                                   /* Return process id to caller     */
}                                                /*                                 */



/*==================================================================================*/
/*                                                                                  */
/* get_tname()                                                                      */
/*                                                                                  */
/*==================================================================================*/

#if 0
void get_tname_(char * tname, int      tname_len)
{
  int         slot;                              /* Slot Number in realtime common  */
  int         index;                             /* Working Index variable          */
  int         nobytes;                           /* Number of bytes for the name    */
  char        name[TASKNAMELEN];                 /* Working name space              */

  slot      = private_data.my_slot - 1L;         /* Get slot number (Make 0 based)  */
  memcpy(name,REALTIME1.shared_names[slot],TASKNAMELEN);  /* Get the name           */
  if(name[0] == '\0') {                          /* Empty slot ?                    */
    memset(name,(int     ) '?', TASKNAMELEN);    /* Insert question marks           */
  }                                              /*                                 */
  nobytes = ((tname_len < TASKNAMELEN) ? tname_len : TASKNAMELEN); /* Bytes to move */
  memcpy(tname,name,nobytes);                    /* Move the data                   */
  if(tname_len > TASKNAMELEN) {                  /* Do we need to fill out spaces   */
    for(index=nobytes;index<tname_len;index++) { /* Fill out the spaces             */
      tname[index] = ' ';                        /*                                 */
    }                                            /*                                 */
  }                                              /*                                 */

  return;                                        /*                                 */
}                                                /*                                 */
#endif



/*==================================================================================*/
/*                                                                                  */
/* get_slot()                                                                       */
/*                                                                                  */
/* This routine is used to locate an empty task slot or to locate a task slot that  */
/* has been allocated.  The taskname argument "tname" is used.                      */
/*                                                                                  */
/* Returns the slot number or -1 if the an active process is found by the same name */
/* This will prevent inadvertent removal of a process -jeffs 11/14/08               */
/*                                                                                  */
/*==================================================================================*/

int      get_slot_(char * tname, int      tname_len)
{
  int          index;                            /* Working index                   */
  int          notfree;                          /* Test & Set logical flag         */
  int          status;                           /* Working status                  */
  int          slot;                             /* Slot to return to caller        */
  int          fifteen;                          /* Value for bit 15                */
  char         tskname[TASKNAMELEN];             /* Working taskname                */
  char         empname[TASKNAMELEN];             /* Working taskname                */

  fifteen = 15L;                                 /* Bit for test and set            */
  slot    =  0L;                                 /* Indicate no slot                */
  memset(tskname,' ', TASKNAMELEN);              /* Clear the taskname buffer       */
  memset(empname,'\0',TASKNAMELEN);              /* Clear the empty name buffer     */
  if(tname[0] == '\0') {                         /* We have an empty name           */
    memset(tskname,'\0',TASKNAMELEN);            /* Insure all of it is zero'ed     */
  }                                              /*                                 */
  index = ((tname_len < TASKNAMELEN) ? tname_len /* Move either the length of string*/
                                 : TASKNAMELEN); /* or the maximum name length      */
  memcpy(tskname,tname,index);                   /* Move the taskname               */

  for(index=0;index<SHARED_SLOTs;index++) {      /* Check each possible slot        */

    /* Is this taskname the same as that requested ................................ */
    status = memcmp(tskname,REALTIME1.shared_names[index],TASKNAMELEN);
    if(status == 0L) {                           /* name is found */

      /* Requesting an empty slot? ................................................ */
      status = memcmp(tskname,empname,TASKNAMELEN);
      if(status == 0L) {

        /* Is this the free slot to allocate ...................................... */
        notfree = (int     ) test_and_set_(&REALTIME1.shared_locks[index],&fifteen);
        if(! notfree) {

          /* Give this slot to the caller ......................................... */
          slot = index + 1L;
          break;
        }
      }

      /* We found the entry in the table .......................................... */
      else {
        int pid = REALTIME1.shared_pid[index];
        if (pid > 0) {
          if (kill(pid ,0x0000) == 0) {     
           /* this pid is in use so cannot load the process! */
            slot = -(index + 1L);     /* return the negative as an indicator        */
            break;
          }
        }
        /* Give this slot to the caller ........................................... */
        slot = index + 1L;
        break;
      }
    }
  }
  return(slot);
}



/*==================================================================================*/
/*                                                                                  */
/* clear_slot                                                                       */
/*                                                                                  */
/* This function returns the following possible values to the caller                */
/*                                                                                  */
/*   1 - SHARED_SLOTs - The slot number that was cleared                            */
/*  -1 - The specified slot number is out of the valid range                        */
/*  -2 - The specified slot is already empty                                        */
/*                                                                                  */
/*==================================================================================*/

int      clear_slot_(int      * slot_to_clear)
{
  int              slot;                         /* Slot to be cleared              */
  int              rtnstat;                      /* Return status                   */
  int              status;                       /* General working status          */
  char             empname[TASKNAMELEN];         /* Working taskname                */
//  int              retval;                        /*jeffsun 25Feb2001*/ //tt82513
  
  memset(empname,'\0',TASKNAMELEN);              /* Clear the empty name buffer     */

  if((*slot_to_clear) == 0L) {                   /* Clear my task information slot  */
    slot = private_data.my_slot - 1L;            /* Get the slot (0 based)          */
  }                                              /*                                 */
  else {                                         /* Clear a specified slot          */
    slot = (*slot_to_clear) - 1L;                /* Get the slot (0 based)          */
  }                                              /*                                 */

  if((slot < 0L) || (slot >= SHARED_SLOTs)) {    /* Slot is out of range            */
    rtnstat = -1L;                               /* Indicate error                  */
    goto exit_here;                              /* Go to the common exit point     */
  }                                              /*                                 */
 
  status = memcmp(empname,REALTIME1.shared_names[slot],TASKNAMELEN);
  if(status == 0L) {                              /* Table slot is empty             */
    rtnstat = -2L;                               /* Indicate error                  */
    goto exit_here;                              /* Go to the common exit point     */
  }                                              /*                                 */

// as per rich add this debug to debug missing name in table. jeffs 10/22/08 
  if (REALTIME1.shared_pid[slot] > 0)
  {
    dLog(DLOG_DEBUG_9, " Note: Clearing slot #%d name=<%.*s> pid=%d", 
        slot+1 ,                       /* slot as displayed by DM */
        8,REALTIME1.shared_names[slot],
        REALTIME1.shared_pid[slot]);
  }

  memcpy(REALTIME1.shared_names[slot],empname,TASKNAMELEN);
  REALTIME1.shared_pid[slot]    = 0L;
  REALTIME1.shared_mqid[slot]   = 0L;
  REALTIME1.shared_gid[slot]    = 0L;
  REALTIME1.shared_uid[slot]    = 0L;
  REALTIME1.shared_ppid[slot]   = 0L;
//  retval = test_and_reset(&REALTIME1.shared_locks[slot]); //tt82513
  test_and_reset(&REALTIME1.shared_locks[slot]);            //tt82513
  
  rtnstat  = slot + 1L;

  exit_here:

  return(rtnstat);

}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
