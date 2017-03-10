/************************************************************************************/
/*                                                                                  */
/* gettslot_info.c    Get Information for a Task Table Slot               R00-00.00 */
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
/*   The status value returned are :  -1  = Bad Slot Value                          */
/*                                     0  = Empty Slot                              */
/*                                    +1  = Filled Slot                             */
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
/* R00-00.00  DRLJr        04/Apr/1998  File created.                               */
/* R01-00.00 jeffsun       21/Feb/2001  Replace realtime1 with REALTIME1 for sparc  */
// tt60135    GJ           02/Mar/2011  Replace obsolete realtime1.h with gb1_init.h
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "gb1_init.h"



/*==================================================================================*/
/*                                                                                  */
/* Get_TName()                                                                      */
/*                                                                                  */
/*==================================================================================*/

int      Get_TSlot_Info(int        tslot, unsigned char * tname, int      tname_len,
                        int      * tpid)
{
  int         status;                            /* General Status                  */
  int         slot;                              /* Slot Number in realtime common  */
  int         index;                             /* Working Index variable          */
  int         nobytes;                           /* Number of bytes for the name    */
  char        name[TASKNAMELEN];                 /* Working name space              */

  slot      = tslot - 1L;                        /* Adjust slot to 0 based          */
  if((slot >= SHARED_SLOTs) || (slot < 0L)) {    /* Report an error                 */
    status = -1L;                                /* Bad Slot Number                 */
    goto return_to_caller;                       /* Return to the caller            */
  }                                              /*                                 */

  memcpy(name,REALTIME1.shared_names[slot],TASKNAMELEN);   /* Get the name          */
  if((name[0] == '\0') || (name[0] == ' ')) {    /* Empty slot ?                    */
    status =  0L;                                /* Empty Slot                      */
  }                                              /*                                 */
  else {
    nobytes = ((tname_len < TASKNAMELEN) ? tname_len : TASKNAMELEN);/* Bytes to move*/
    memcpy(tname,name,nobytes);                  /* Move the data                   */
    if(tname_len > TASKNAMELEN) {                /* Do we need to fill out spaces   */
      for(index=nobytes;index<tname_len;index++) { /* Fill out the spaces           */
        tname[index] = ' ';                      /*                                 */
      }                                          /*                                 */
    }                                            /*                                 */
    (*tpid) = REALTIME1.shared_pid[slot];        /* Return the PID                  */
    status  = 1L;                                /* Values returned                 */
  }                                              /*                                 */

  return_to_caller :                             /* Return to the caller            */

  return(status);                                /*                                 */
}                                                /*                                 */

void get_tslot_info_(int      * tslot,  char     * tname,     int      * tpid, 
                     int      * status, int        tname_len)
{
  (*status) = Get_TSlot_Info(*tslot,(unsigned char *)tname,tname_len,tpid);
  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
