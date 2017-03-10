/****************************************************************************/
/*                                                                          */
/* cirlist_lock.c  Lock a Circular List                           R00-00.00 */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Programing Notes:                                                        */
/*                                                                          */
/* This routine is a replacement for the list.f subroutine.                 */      
/*                                                                          */
/* The test program compile line for the list lock and unlock routines is   */
/*                                                                          */
/* cc -o cirlist_lock -DMAIN    cirlist_lock.c  cirlist_unlock.o  tset.o \  */
/*                              delst.o         msleep.o                    */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Revision History:                                                        */
/*                                                                          */
/* Revision   Date         Whom      Why                                    */
/* =========  ===========  ========  ====================================== */
/* R00-00.00  15/Apr/1999  DRLJr     Replacement for list.f                 */  
/* R00-02.00  20/Feb/2001  PT        Mods for Sparc posix locks             */
/* R00-02.01  30/Jul/2001  PT        Changed lock variables to posix_mlock_t*/
// tt80093    22/May/2014  gj        Clean up prototypes
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include "list.h"
#include "posix_mlock.h"
#include "dlogdefs.h"
//#include "libos32.h"    //tt80093
#include "portability.h"        //60119
#include "proto_common.h" //tt80093


#define TARGET_WAIT 50000    // 50 millisec
static void spin_awhile(int i);
#define  USHRT  unsigned short
#define  SLONG  int         

void get_tname_(char * name, int      length);

const char *   list_names[] = { "UNK ", "ATL ", "RTL ", "ABL ", "RBL ",
                                         "ATLL", "RTLL", "ABLL", "RBLL" };
const char *   lock_msgs[]  = {
                    "Unknown queue lock state indicated.  State unknown.",
                    "Queue appears to have been left locked.  Lock ignored.",
                    "Queue lock time out - Lock being ignored - OVERRIDE."
                };
/* static char     taskname[]   = { "           " }; */      /* PT - 02/20/01 */
static unsigned char     taskname[]   = { "           " };   /* PT - 02/20/01 */
const char *   fmt10000     = {
                    " %-8.8s %-4.4s %4hX %4hX %4hX %4hX %8X %8X"
                };
const char *   fmt10001     = {
                    " %-8.8s %s"
                };

/*==========================================================================*/
/* cirlist_lock ("C" Interface)                                             */
/*==========================================================================*/

int      cirlist_lock(cirlist * list, unsigned int      type, int      param)
{
  int                  cc;                 /* Return status/condition code  */
  int                  plocked;            /* Prior Lock State              */
  int                  lock_bit;           /* The bit to use for the lock   */
  int                  loop;               /* Delay Loop Counter            */
  int                  code;               /* Lock error code               */
  int                  rtn;                /* Routine Code                  */
  posix_mlock_t      * lock;               /* Address of the lock           */ /* PT - 07/30/01 */
  unsigned short        nslots;             /* Number of slots               */
  unsigned short        nused;              /* Number of used slots          */
  unsigned short        curtop;             /* Current top slot              */
  unsigned short        nxtbot;             /* Next bottom slot              */
  unsigned short        old_used = 0;       /* Old Used Value                */
  unsigned long long    snap, elapsed;
  /*------------------------------------------------------------------------*/
  /* Perform the needed setup to lock the list                              */
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
    nused    = list->nused;                /* Number of slots used          */
  }                                        /*                               */
  else if((type >= 5) && (type <= 8)) {    /* Lockable lists                */
    lock     = &((cirlistl *) list)->lock; /* Address of the lock field     */
    nused    = ((cirlistl *) list)->nused; /* Number of slots used          */
  }                                        /*                               */
  else {                                   /* Unknown list command          */
    rtn        = 0L;                       /* Set to unknown                */
    code       = 0L;                       /* And the error message         */
    goto process_error;                    /* Report the error              */
  }                                        /*                               */
#endif                                     /* dgux                          */

  /*------------------------------------------------------------------------*/
  /* Lock the list                                                          */
  /*------------------------------------------------------------------------*/

  snap=GETHRTIME();                          //60119
  for(loop=0;loop<10000;loop++) {            /* Loop until we have the lock   */
     plocked= test_and_set_(lock,&lock_bit);  /* Lock the list                 */
     if(! plocked) {                      /* It is ours now                */
        cc  = CC_Z;                        /* Tell caller we have lock      */
        if (loop>0)  {
          elapsed = (GETHRTIME()-snap)/1000;  //60119
          dLog(DLOG_WARNING,"GOT lock after %d tries (%lld usec)!",
            loop, elapsed);
        }
        goto return_to_caller;             /* Return to the caller          */
     }                                    /*                               */
     else {
       elapsed = (GETHRTIME()-snap)/1000;  //60119
       if (elapsed > TARGET_WAIT) {
         /* override the lock */
         dLog(DLOG_MAJOR,"Retries Exhausted! Lock released after %d attempts (%lld usec)." ,
            loop,elapsed);
         break;
       }
       else {
         spin_awhile(loop);
       }
     }
  }                                      /*                               */

  if(nused == old_used) {                /* Number of used slots the same */
      code = 1L;                           /* Indicate apparent problem     */
      cc   = CC_Z;                         /* Tell caller we have lock      */
      goto process_error;                  /* Report the error              */
  }                                      /*                               */
  if(LOCK_OVERRIDE) {                    /* Override the lock             */
      code = 2L;                           /* Say we took the lock          */
      cc   = CC_Z;                         /* Tell caller we have lock      */
      goto process_error;                  /* Report the error              */
  }                                      /*                               */
  else {                                 /* Override is off               */
    cc = CC_C;                           /* Indicate lock is on           */
  }                                      /*                               */
  goto return_to_caller;                   /* Return the list routines      */

  /*------------------------------------------------------------------------*/
  /* Report an error                                                        */
  /*------------------------------------------------------------------------*/

  process_error :

  if(taskname[0] == ' ') {
    get_tname_((char *)taskname,8L);
  }

  nslots = list->nslots;
  nused  = list->nused;
  curtop = list->curtop;
  nxtbot = list->nxtbot;

  dLog(DLOG_MAJOR,fmt10000,taskname,list_names[rtn],
                                   nslots,  nused, curtop,
                                   nxtbot,  list,  param);
  dLog(DLOG_MAJOR,fmt10001,taskname,lock_msgs[code]);
  
  /*------------------------------------------------------------------------*/
  /* Return to the caller                                                   */
  /*------------------------------------------------------------------------*/

  return_to_caller :

  return(cc);
}

int x_cirlist_lock;
//do not optimize this routine since we want the overhead to be fixed// 
#ifndef __linux__
#pragma opt 0 (spin_awhile)
#endif
static void spin_awhile(int i)
{
     int j;
     //x_cirlist_lock is external so is not optimized away.//
     for (x_cirlist_lock=0,j=0; j<5000; j++)
     {
        x_cirlist_lock+=((j*2+1-i)+(i*2));
     }
}

/*==========================================================================*/
/* Main Test Program                                                        */
/*==========================================================================*/

#ifdef MAIN

void get_tname_(unsigned char * name, int      length)
{
  for(length=length-1;length>=0;length--) {
    name[length] = 'A';
  }
  return;
}

int      main(int      argc, char ** argv)
{
/*   int          list[7]; */                                /* PT - 02/20/01 */
  /* size of circular list is # slots */                     /* PT - 02/20/01 */
  /* + 2 int  words for list control */                      /* PT - 02/20/01 */
  /* + 6 int  wds for posix lock */                          /* PT - 02/20/01 */
  int          list[15];                                     /* PT - 02/20/01 */
  int          cc;
  cirlist    * alist;

  alist    = (cirlist *) &list[0];
  deflst(alist,5L);

  cc       = cirlist_lock(alist,LIST_ATL,0);
  dLog(DLOG_MAJOR,"%02X = cirlist_lock(0x%08X,LIST_ATL,0), ",cc,alist);
  dLog(DLOG_MAJOR,"%4hX %4hX %4hX %4hX",alist->nslots,alist->nused,
                                       alist->curtop,alist->nxtbot);
  dLog(DLOG_MAJOR,"\n");

  cc       = cirlist_lock(alist,LIST_RBL,0);
  dLog(DLOG_MAJOR,"%02X = cirlist_lock(0x%08X,LIST_RBL,0), ",cc,alist);
  dLog(DLOG_MAJOR,"%4hX %4hX %4hX %4hX",alist->nslots,alist->nused,
                                       alist->curtop,alist->nxtbot);
  dLog(DLOG_MAJOR,"\n");


  cc       = cirlist_unlock(alist,LIST_RBL);
  dLog(DLOG_MAJOR,"%02X = cirlist_lock(0x%08X,LIST_RBL,0), ",cc,alist);
  dLog(DLOG_MAJOR,"%4hX %4hX %4hX %4hX",alist->nslots,alist->nused,
                                       alist->curtop,alist->nxtbot);
  dLog(DLOG_MAJOR,"\n");

  cc       = cirlist_unlock(alist,LIST_ABL);
  dLog(DLOG_MAJOR,"%02X = cirlist_lock(0x%08X,LIST_ABL,0), ",cc,alist);
  dLog(DLOG_MAJOR,"%4hX %4hX %4hX %4hX",alist->nslots,alist->nused,
                                       alist->curtop,alist->nxtbot);
  dLog(DLOG_MAJOR,"\n");

  exit(0L);
}

#endif

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
