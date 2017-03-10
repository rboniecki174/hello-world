/************************************************************************************/
/*                                                                                  */
/* msleep.c     Perform a micro-second based sleep                        R00-02.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines to perform a micro-second based time sleep.      */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   ticks  - This is the address of int      (INTEGER*4) than contains the number  */
/*          - of milliseconds to sleep.                                             */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*  These routines are intended to be called from FORTRAN code or "C" code that     */
/*  acts like FORTRAN code.                                                         */
/*                                                                                  */
/*  Under DG/UX the timeout setting are not updated so there is no way to check and */
/*  see if we should some more.                                                     */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-01.00  DRLJr        01/Apr/1998  Header file added and code changed to be    */
/*                                      able to handle value greater than 999.      */
/*                                      Code was converted to use ANSI prototypes.  */
/* R00-02.00  DRLJr        24/Nov/1998  Added "C" interface (pass by value)         */
// tt80093    gj           22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
//#include <libos32.h>    //tt80093
#include "proto_common.h" //tt80093

static struct timeval      timewait;
#ifdef MAIN
static int                 select_status;
static int                 select_errno;
#endif


/*==================================================================================*/

void millisleep(const int      ticks)
{
  millisleep_(&ticks);
  return;
}

/*==================================================================================*/

void millisleep_(const int      * ticks)
{
  int                  seconds;             /* Number of whole seconds to wait      */
  int                  mseconds;            /* Number of milliseconds to wait       */
  int                  useconds;            /* Number of microseconds to wait       */
  int                  nseconds;            /* Number of nanoseconds to wait        */
  int                  status;              /* Timer status calls                   */
  char               * errstr;              /* Pointer to an error string           */

  /*--------------------------------------------------------------------------------*/
  seconds               = (*ticks) / 1000;  /* Number of complete seconds           */
  mseconds              = (*ticks) % 1000;  /* Number of milliseconds less seconds  */
  useconds              = mseconds * 1000;  /* Number of microseconds to wait       */
  nseconds              = mseconds * 1000000;  /* Number of nanoseconds to wait     */
/*  fprintf(stderr,"%s: wticks=%d\n", __FILE__,*ticks); */
  timewait.tv_sec              = seconds;   /* Number of seconds to wait            */
  timewait.tv_usec             = useconds;  /* Number of microseconds to wait       */

  /*--------------------------------------------------------------------------------*/

  status = select(0L,(fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL, &timewait);
  if((status < 0) && (errno != EINTR)) {
    errstr = strerror(errno);
    fprintf(stdout,"Error %d in millisleep_ (msleep.c) : %s\n",errno,errstr);
  }

#ifdef MAIN
  select_status = status;
  select_errno  = errno;
#endif

  return;
}


#ifdef MAIN

void               (*trphandler)();
void               trap_handler();
time_t             trp_time;

int      main(long argc, char ** argv)
{
  int          index;
  time_t       timebefore;
  time_t       timeafter;
  int          milliseconds;

  if(argc <= 1) {
    fprintf(stderr,"At least 1 milliseconds count must be specified.\n");
    exit(1);
  }

  trphandler = signal(SIGINT     , trap_handler);

  for(index=1;index<argc;index++) {
    milliseconds = atol(argv[index]);
    trp_time   = (time_t) 0L;
    timebefore = time((time_t *) NULL);
    (void) millisleep_((const int *)&milliseconds);
    timeafter  = time((time_t *) NULL);
    fprintf(stderr,"\n");
    fprintf(stderr,"Before->Trap->After  : %12d -> %12d -> %12d \n",
                    timebefore,trp_time,timeafter);
    fprintf(stderr,"  Select   (Stop) : %8d %8d\n",
                      timewait.tv_sec, timewait.tv_usec);
    fprintf(stderr,"  Select   (Stat) : %8d %8d\n",
                      select_status,select_errno);
  }

  exit(0);

}

void trap_handler(int      signo)
{
  trp_time = time((time_t *) NULL);
  return;
}

#endif 

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
 
