/*******************************************************************************/
/*                                                                             */
/* set_realtime.c  Garban Broker System Initialization               R00-00.00 */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Programing Notes:                                                           */
/*                                                                             */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Revision History:                                                           */
/*                                                                             */
/* Revision   Date         Whom      Why                                       */
/* =========  ===========  ========  ========================================= */
/* R00-00.00  20/Oct/1999  DRLJr     Routine adapted from gb1_init.c           */
/* R01-00.00 jeffsun       21/Feb/2001  Replace realtime1 with REALTIME1 for sparc  */
// tt60135    03/02/11     GJ        Replace obsolete realtime1.h with gb1_init.h
/*                                                                             */
/*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "private_data.h"
#include "gb1_init.h"
#include "devicecodes.h"

#include "process_asn.h"

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef LINT
#define LINT   int     
#endif

#ifndef STRMIN
#define STRMIN(x,y) ((x < y) ? x : y)
#endif

#define MY_SLOT           private_data.my_slot
#define CMY_SLOT         (private_data.my_slot - 1L)
#define BASE_TASK_NAME    (UCHAR *) private_data.base_task_name
#define SHARED_NAMES      REALTIME1.shared_names

/*=============================================================================*/
/* Prototypes for FORTRAN Functions and other functions                        */
/*=============================================================================*/

int      get_slot_(unsigned char * name, int      name_len);
int      dbg_queue_state_(void);
void     set_signals_(void);
int      mq_dele_(int      * mid);

/*=============================================================================*/
/* gb1_init (FORTRAN Interface)                                                */
/*=============================================================================*/

int        set_realtime_()
{
  int              status;                    /* General working status        */
  int              zero;                      /* A general working zero value  */
  int              index;                     /* Working index                 */
  int              my_key;                    /* Shared memory segment key     */
  unsigned char    empty[8];                  /* Working taskname              */

  /*===========================================================================*/
  /* Do the basic initialization processing                                    */
  /*===========================================================================*/

  IF_DEBUG(1)
   fprintf(stderr,"set_realtime - Initializing\n");
  END_DEBUG

  zero                            = 0L;
  status                          = 0L;
  MY_SLOT                         = 0L;
  memset(empty, 0L, 8L);
  
  /*===========================================================================*/
  /* Set up the realtime common block with the taskname                        */
  /*===========================================================================*/

  MY_SLOT     = 0L;                                     /* Clear to 0          */

  MY_SLOT     = get_slot_(BASE_TASK_NAME,8L);           /* In table already    */
  if(MY_SLOT == 0L) {                                   /* No, need a slot     */
    MY_SLOT   = get_slot_(empty,8L);                    /* Get a slot          */
    if(MY_SLOT != 0L) {                                 /* Have a slot         */
      memcpy(SHARED_NAMES[CMY_SLOT],BASE_TASK_NAME,8L); /* Move the name over  */
    }
  }
  else if (MY_SLOT < 0L) {               /* there is a valid process in that slot.  REJECT! */
     fprintf(stderr,"ERROR: Unable to load process with this taskname.\n");
     fprintf(stderr,"Slot #%d is found with an active program [%.*s]\n",
           abs(MY_SLOT), 8, BASE_TASK_NAME);
     fprintf(stderr,"Kill it first or load this process with a different taskname\n");
     exit(253L);
  }

#if 1
  fprintf(stderr,"My Slot is : %d\n",MY_SLOT);
  fprintf(stderr,"My Name (Base Task Name) is : <%.8s>\n",BASE_TASK_NAME);
  fprintf(stderr,"My Name (Shared_Names[%d]) is : <%.8s>\n", (int)CMY_SLOT, SHARED_NAMES[CMY_SLOT]);
#endif

  if (MY_SLOT == 0L) {                                   /* Look it up again    */
    MY_SLOT   = abs(get_slot_(BASE_TASK_NAME,8L));      /* In table already    */
  }                                                     /*                     */


  if(MY_SLOT == 0L) {                                   /* Look it up again    */
    fprintf(stderr," SET_REALTIME  : CAN NOT FIND TASK %-8.8s\n",BASE_TASK_NAME);
    exit(254L);
  }                                                     /*                     */

  /*===========================================================================*/
  /* Set up the realtime common block                                          */
  /*===========================================================================*/

  REALTIME1.shared_pid[CMY_SLOT]     = getpid();        /* Get the PID         */
  REALTIME1.shared_gid[CMY_SLOT]     = getgid();        /* Get the GID         */
  REALTIME1.shared_uid[CMY_SLOT]     = getuid();        /* Get the UID         */
  REALTIME1.shared_pid[CMY_SLOT]     = getpid();        /* Get the PID         */
  REALTIME1.shared_ppid[CMY_SLOT]    = getppid();       /* Get the PPID        */

  my_key = (getuid() * SHARED_SLOTs) + MY_SLOT - 1L + getpid();

  fprintf(stderr," %-8.8s : (%8d%8d%8d%8d) Activated for Real-Time\n",
                  BASE_TASK_NAME,REALTIME1.shared_pid[CMY_SLOT],
                                 REALTIME1.shared_mqid[CMY_SLOT],
                                 MY_SLOT,
                                 my_key);
  
  if (REALTIME1.shared_mqid[CMY_SLOT] > 0)
  {
    fprintf(stderr," Deleting Message Queue %d remaining in the task table.\n", 
         REALTIME1.shared_mqid[CMY_SLOT]);

    mq_dele_(&REALTIME1.shared_mqid[CMY_SLOT]);
  }

  index  = dbg_queue_state_();
  if(index == 0L) {
   fprintf(stderr," %-8.8s : Queue Parameter Debugging Disabled\n",BASE_TASK_NAME);
  }
  else {
    fprintf(stderr," %-8.8s : Queue Parameter Debugging Enabled\n",BASE_TASK_NAME);
  }  

  /*===========================================================================*/
  /* Set the signal to a known safe state                                      */
  /*===========================================================================*/

  set_signals_();
  
  /*===========================================================================*/
  /* Return to the caller                                                      */
  /*===========================================================================*/

  status = 0L;
  return(status);
}

/*******************************************************************************/
/*                                                                             */
/*******************************************************************************/
