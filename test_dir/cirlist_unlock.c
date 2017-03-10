/****************************************************************************/
/*                                                                          */
/* cirlist_unlock.c  Unlock a Circular List                       R00-00.00 */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Programing Notes:                                                        */
/*                                                                          */
/* This routine is a replacement for the list.f subroutine.                 */      
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Revision History:                                                        */
/*                                                                          */
/* Revision   Date         Whom      Why                                    */
/* =========  ===========  ========  ====================================== */
/* R00-00.00  15/Apr/1999  DRLJr     Replacement for list.f                 */
/* R00-02.00  20/Feb/2001  PT        Mods for Sparc posix locks             */
/* R00-00.01  30/Jul/2001  PT        Changed lock variables to posix_mlock_t*/
/* 72128      2012/01/16   jeffs     const char array passed to get_tname   */ 
/*                                   causes a core dump                     */
// tt80093    2014/05/22   gj        Clean up prototypes
// tt82153    2014/11/12   gj        Change test_and_reset/test_and_reset_
//                                   to return void
/****************************************************************************/

#include <stdio.h>
#include "list.h"
#include "posix_mlock.h"
#include "dlogdefs.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define  USHRT  unsigned short
#define  SLONG  int         

static const char *   list_names[] = { "UNK ", "ATL ", "RTL ", "ABL ", "RBL ",
                                         "ATLL", "RTLL", "ABLL", "RBLL" };
static const char *   lock_msgs[]  = {
                    "Unknown queue lock state indicated.  State unknown.",
                    "Queue appears to have been already unlocked.",
                };
//72128static const char     taskname[]   = { "           " };
static char     taskname[]   = { "           " };                        //72128
static const char *   fmt10000     = {
                    " %-8.8s %-4.4s %4hX %4hX %4hX %4hX          %8X"
                };
static const char *   fmt10001     = {
                    " %-8.8s %s"
                };

void get_tname_(char * name, int      length);

/*==========================================================================*/
/* cirlist_lock ("C" Interface)                                             */
/*==========================================================================*/

int      cirlist_unlock(cirlist * list, unsigned int      type)
{
  int                  cc;                 /* Return status/condition code  */
  int                  code;               /* Error message selection code  */
//  int                  plocked;            /* Prior Lock State              */ //tt82153
  int                  lock_bit;           /* The bit to use for the lock   */
  unsigned short        nslots;             /* Number of slots               */
  unsigned short        nused;              /* Number of used slots          */
  unsigned short        curtop;             /* Current top slot              */
  unsigned short        nxtbot;             /* Next bottom slot              */
  int                  rtn;                /* Routine Code                  */
/*unsigned short      * lock; */            /* Address of the lock           */ /* PT - 02/20/01 */
/*pthread_mutex_t    * lock; */            /* Address of the lock           */ /* PT - 02/20/01 */
  posix_mlock_t      * lock;               /* Address of the lock           */ /* PT - 07/30/01 */

  /*------------------------------------------------------------------------*/
  /* Perform the needed setup to unlock the list                            */
  /*------------------------------------------------------------------------*/

  cc         = CC_Z;                       /* Set the condition code        */
  rtn        = type;                       /* Get the routine type          */
  lock_bit   = LOCK_BIT;                   /* Get lock bit to use           */
#ifdef posix                               /* PT - 02/20/01                 */
  lock = &list->lock;
  nused    = list->nused;                  /* Number of slots used          */
  if(!((type >= 1) && (type <= 8))) {      /* Bad list types                */
    rtn        = 0L;                       /* Set to unknown                */
    code       = 0L;                       /* And the error message         */
    goto process_error;                    /* Report the error              */
  }
#endif                                     /* posix                         */

#ifdef dgux                                /* PT - 02/20/01                 */
  if((type >= 1) && (type <= 4)) {         /* Standard lists                */
    lock     = &(list->nslots);            /* Address of psuedo lock field  */
  }                                        /*                               */
  else if((type >= 5) && (type <= 8)) {    /* Lockable lists                */
    lock     = &((cirlistl *) list)->lock; /* Address of the lock field     */
  }                                        /*                               */
  else {                                   /* Unknown list command          */
    rtn        = 0L;                       /* Set to unknown                */
    code       = 0L;                       /* And the error message         */
    goto process_error;                    /* Report the error              */
  }                                        /*                               */
#endif                                     /* dgux                          */

  /*------------------------------------------------------------------------*/
  /* Unlock the list                                                          */
  /*------------------------------------------------------------------------*/

//  plocked= test_and_reset_(lock,&lock_bit);/* Lock the list            */ //tt82153
//  if(! plocked) {                          /* List is already unlocked */ //tt82153
//    code       = 1L;                       /* And the error message    */ //tt82153
//    goto process_error;                    /* Report the error         */ //tt82153
//  }                                        /*                          */ //tt82153
  test_and_reset_(lock,&lock_bit);         /* Unlock the list            */ //tt82153
  goto return_to_caller;                   /* Return the list routines   */

  /*------------------------------------------------------------------------*/
  /* Report an error                                                        */
  /*------------------------------------------------------------------------*/

  process_error :

  if(taskname[0] == ' ') {
    get_tname_((char *)taskname,8L);
  }
#ifdef posix                               /* PT - 02/20/01                 */
  nslots = list->nslots;
  nused  = list->nused;
  curtop = list->curtop;
  nxtbot = list->nxtbot;
#endif                                     /* posix                         */

#ifdef dgux                                /* PT - 02/20/01                 */
  if((type >= 0) && (type <= 4)) {         /* Standard lists and Unknown    */
    nslots = list->nslots;
    nused  = list->nused;
    curtop = list->curtop;
    nxtbot = list->nxtbot;
  }                                        /*                               */
  else if((type >= 5) && (type <= 8)) {    /* Lockable lists                */
    nslots   = ((cirlistl *) list)->nslots;
    nused    = ((cirlistl *) list)->nused;
    curtop   = ((cirlistl *) list)->curtop;
    nxtbot   = ((cirlistl *) list)->nxtbot;
  }                                        /*                               */
  else {
    type     = 0L;
  }
#endif                                     /* dgux                          */
   dLog(DLOG_MAJOR,fmt10000,taskname,list_names[rtn],
                                   nslots,  nused, curtop, 
                                   nxtbot,  list);
   dLog(DLOG_MAJOR,fmt10001,taskname,lock_msgs[code]);

  
  goto return_to_caller;

  /*------------------------------------------------------------------------*/
  /* Return to the caller                                                   */
  /*------------------------------------------------------------------------*/

  return_to_caller :

  return(cc);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
