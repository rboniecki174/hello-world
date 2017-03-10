/************************************************************************************/
/*                                                                                  */
/* realtime1.h      "C" verion of the realtime1 common block              R00-01.03 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the definition of the realtime1 common block.                 */
/*                                                                                  */
/* Tables are generally access with slot information based upon FORTRAN.  For "C"   */
/* the slot number must be offset to a 0 ("C") based value versus the 1 (FORTRAN)   */
/* based value that is normally maintained for the slot number.                     */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/* --NONE--                                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        12/Mar/1998  New file                                    */
/* R00-01.00  DRLJr        07/Apr/1999  Added various #if/#endif structures to make */
/*                                      include file more versatile and useful.     */
/* R01-01.00  jeffsun      21/Feb/2001  Ported to Sun add conditional for underscore*/
/*                                      Implement shared_lock field for pthreads_mutex */
/* R01-01.01  PT           07/Jun/2001  Added Neworder thread monitor flags        */
/* R01-01.02  PT           30/Jul/2001  Changed lock variables to posix_mlock_t    */
/* R01-01.03  PT           12/Dec/2001  Added txnd server health variables         */
/* R01-01.04  jeffs        17/Dec/2001  Added neworder_init_done                   */
/************************************************************************************/

#ifndef __REALTIME1_H__
#define __REALTIME1_H__

#define  SHARED_SLOTs     300
#define  MUTEX_LOCK_SIZE  5

#ifndef  TASKNAMELEN
#define  TASKNAMELEN        8
#endif

#include <sys/types.h>
#include <posix_mlock.h>                                     /* PT - 07/30/01 */



typedef  struct cmn_realtime1 {                    /* Structure of the realtime common */
  int              shared_pid[SHARED_SLOTs];       /* Process pids                     */
  char             shared_names[SHARED_SLOTs][TASKNAMELEN];  /* Registered task names  */
  int              shared_mqid[SHARED_SLOTs];      /* Message Queues                   */
  int              shared_gid[SHARED_SLOTs];       /* Process gids                     */
  int              shared_uid[SHARED_SLOTs];       /* Process uid owner                */
  int              shared_ppid[SHARED_SLOTs];      /* Process parent pid               */
/*  pthread_mutex_t  shared_locks[SHARED_SLOTs]; */  /* Shared Locks                     */ /*jeffsun 22Feb2001*/
  posix_mlock_t    shared_locks[SHARED_SLOTs];     /* Shared Locks                     */ /* PT - 07/30/01 */
#if 1               /* PT 06/07/01  Added neworder queue & 12/12/01 txnd server monitoring */
  int              neworder_od_ok,                 /* Order depth queue valid */
                   neworder_oh_ok,                 /* Order history queue valid */
                   neworder_th_ok,                 /* Trade history queue valid */
                   neworder_va_ok;                 /* Vacancy queue valid  */
  int              neworder_od_count,              /*Order depth queue xfer count */
                   neworder_oh_count,              /*Order history queue xfer count */
                   neworder_th_count,              /*Trade history queue xfer count */
                   neworder_va_count,              /* Vacancy queue xfer count */
                   txnorderd_alive,                /* ODHS txnorderd serever alive */
                   txnhistd_alive,                 /* ODHS txnhistd serever alive */
                   neworder_init_done;             /* Initialization of neworder complete */
  char             EXTRA_RESERVED_SPACE[4052];     /* Extra space                      */
#else               /* PT 06/07/01  Old space allocation */
  char             EXTRA_RESERVED_SPACE[4096];     /* Extra space                      */
#endif               /* PT 06/07/01 */
} cmn_realtime1;                                   /*                                  */


#ifdef dbux
#define REALTIME1 realtime1
#endif

#if defined(__sparc) || defined(__i386)
#define REALTIME1 realtime1_
#endif

#if defined(__i386)
#define DEPTHHDL_IU 1
#endif

#ifdef BLOCK_DATA

           cmn_realtime1      REALTIME1;        /* Realtime Control Data            */

#else

  extern   cmn_realtime1      REALTIME1;        /* Realtime Control Data            */

#endif

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
