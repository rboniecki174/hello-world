/************************************************************************************/
/*                                                                                  */
/* ccwait.c     Perform a time wait operation                             R00-01.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines to perform a sleep operation based upon the      */
/* arguments that are passed to the routine.                                        */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/* Delay   - This is the amount of time based on the units to wait.                 */
/* Unit    - This is the units (minutes, seconds, millseconds etc.) that Delay is   */
/*         - specified in.                                                          */
/* Status  - This is the returned status.                                           */
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
/* R00-01.00  DRLJr        01/Apr/1998  FORTRAN converted to "C" and the proper     */
/*                                      status code returned.                       */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#define  UNIT_MSECONDS               1
#define  UNIT_SECONDS                2
#define  UNIT_MINUTES                3

#define  MAX_MILLISECONDS    268435455
#define  MAX_SECONDS            268435
#define  MAX_MINUTES              4473

#define  STAT_OKAY                   1
#define  STAT_SYSSPACE               2
#define  STAT_BADUNIT                3
#define  STAT_BADDELAY               4

#define  FDNULL      ((fd_set *) NULL)



void ccwait_(int      * Delay, int      * Unit, int      * Status)
{
  int            stat;                    /* General status variable                */
  int            time;                    /* Number of milliseconds to use          */
  struct timeval timewait;                /* select() timewait structure            */

  stat       = 0L;                        /* Set status to initial value            */

  if((*Delay) <= 0L) {
    stat = STAT_OKAY;
  }
  else {
    switch((*Unit)) {
      case UNIT_MSECONDS :
                if((*Delay) > MAX_MILLISECONDS) {
                  stat = STAT_BADDELAY;
                }
                else {
                  time = (*Delay);
                }
                break;
      case UNIT_SECONDS :
                if((*Delay) > MAX_SECONDS) {
                  stat = STAT_BADDELAY;
                }
                else {
                  time = (*Delay) * 1000L;
                }
                break;
      case UNIT_MINUTES :
                if((*Delay) > MAX_MINUTES) {
                  stat = STAT_BADDELAY;
                }
                else {
                  time = (*Delay) * 60L * 1000L;
                }
                break;
      default :
                stat   = STAT_BADUNIT;
                break;
    }
  }



  if(stat == 0L) {
    timewait.tv_sec  = time / 1000L;
    timewait.tv_usec = ((time % 1000L) * 1000L);
    stat             = select(0L,FDNULL,FDNULL,FDNULL,&timewait);
    if(stat < 0L) {
      if(stat == EINTR) {
        stat = STAT_OKAY;
      }
      else {
        stat = STAT_SYSSPACE;
      }
    }
    else {
      stat = STAT_OKAY;
    }
  }

  (*Status) = stat;
  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
