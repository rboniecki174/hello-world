/*****************************************************************************/
/*                                                                           */
/* monitor_rd.c  Read from the defined monitor programs            R00-02.00 */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* Programming Notes                                                         */
/*                                                                           */
/*                                                                           */
/*===========================================================================*/
/*                                                                           */
/* Revision History                                                          */
/*                                                                           */
/* Revision   When        Who       Why                                      */
/* =========  ==========  ========  ======================================== */
/* R00-00.00 20/Mar/1998  DRLJr     Created from cimon_rd.c                  */
/* R00-01.00 06/Oct/1998  DRLJr     Converted routine to use the subprograms */
/*                                  cimon_getfd() and tmon_getfd() to check  */
/*                                  if a cimon/tmon connect is present.      */
/* R00-02.00 02/Mar/1999  DRLJr     Added debug code for testing for socket  */
/*                                  problem testing.                         */
// tt80093   06/Jun/2014  gj        Clean up prototypes
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>

#include <monitor_rd.h>

#include "proto_common.h" //tt80093

#define  DEBUGG          NO          /* Enable the debug code                 */
#define  DEBUG_AREA1    NO          /* Enable specific debug code            */

#define  MAX_RETRYS      50         /* Number of ties to retry               */
#define  TIMER_TOUT      15         /* Number of seconds to wait             */
#define  SELECT_TOUT      5         /* Number of seconds in the select       */
#define  SELECT_TOUTM     0         /* Number of milliseconds in the select  */

//static int           cimon_connected = { -1L };/* Connected flag/fd          */
//static int           tmon_connected = { -1L }; /* TMON file descriptor       */

//void monitor_rd_(int      * UnixFd, char      * bytes,     int      * length, //tt80093
//                 int      * status, int         bytes_len);                   //tt80093
//int      monitor_rd(int      UnixFd, char * bytes, int      length);          //tt80093

//int      select_zerofd(fd_set * fdset);                                       //tt80093
//int      select_tstfd(fd_set * fdset, int      fd);                           //tt80093
//int      select_setfd(fd_set * fdset, int      fd);                           //tt80093

/*===========================================================================*/
/* monitor_rd                                                                */
/*===========================================================================*/

void monitor_rd_(int      * UnixFd, char      * bytes,     int      * length,
                 int      * status, int         bytes_len)
{
  int          stat;

  stat      = monitor_rd((*UnixFd),bytes,(*length));
  (*status) = stat;
  return;
}

int      monitor_rd(int      UnixFd, char * bytes, int      length)
{
  fd_set          rd_set;
  fd_set          ex_set;
  struct timeval  tout;
  int             status;
  int             max_fd;
  int             curr_time;
#if DEBUGG
#if DEBUG_AREA1
  int             stat_rd,stat_ex;
#endif
#endif

  /*===========================================================================*/
  /* Monitor Read loop for reading from the GUI                                */
  /*===========================================================================*/

  for(;;) {

/*  fprintf(stderr,"Top of monitor_rd processing.  UnixFd=%d  %d\n",UnixFd,connected);*/
    
    /*-------------------------------------------------------------------------*/
    /* Set-up the arguments for the select() call                              */
    /*-------------------------------------------------------------------------*/

    max_fd = 0;                                    /* Set to stdin as max fd.  */

    (void) select_zerofd(&rd_set);                 /* Clear the set            */
    (void) select_zerofd(&ex_set);                 /* Clear the set            */

    tout.tv_sec    = SELECT_TOUT;                  /* Set n seconds            */
    tout.tv_usec   = SELECT_TOUTM * 1000;          /* Number of microseconds   */
  
    /*-------------------------------------------------------------------------*/
    /* Add the read() fd to the select if it is speficied                      */
    /*-------------------------------------------------------------------------*/

    if(UnixFd >= 0L) {                             /* Insert into the fdset    */
      (void) select_setfd(&rd_set,UnixFd);         /* Include in the list      */
      (void) select_setfd(&ex_set,UnixFd);         /* Include in the list      */
      if(UnixFd > max_fd) {                        /* Adjust max_fd to include */
        max_fd = UnixFd;                           /* this unix fd value.      */
      }                                            /*                          */
    }                                              /*                          */

    /*-------------------------------------------------------------------------*/
    /* Insure the value of max_fd includes all of the specified fd's           */
    /*-------------------------------------------------------------------------*/

    max_fd++;                                      /* Adjust for the select()  */
  
    /*-------------------------------------------------------------------------*/
    /* Wait for one of the unix fd's to have data to read                      */
    /*-------------------------------------------------------------------------*/

    status = select(max_fd,&rd_set,NULL,&ex_set,&tout);  /* Wait for data      */
#if DEBUGG
    fprintf(stderr,"Select of monitor_rd processing.  UnixFd=%d, status=%d, errno=%d\n",
                    UnixFd,status,errno); 
#if DEBUG_AREA1
    if(UnixFd >= 0L) {
      stat_rd = ((select_tstfd(&rd_set,UnixFd) != 0) ? 1 : 0);
      stat_ex = ((select_tstfd(&ex_set,UnixFd) != 0) ? 1 : 0);
#if 0
      fprintf(stderr,"  Select of fd %3d (UnixFd) is rd=%1d, ex=%1d\n",
              UnixFd,stat_rd,stat_ex);
#endif
    }
#endif
    sleep(1L);
#endif

    /*-------------------------------------------------------------------------*/
    /* Did we get an error - if EINTR then restart the select                  */
    /*-------------------------------------------------------------------------*/

    if(status < 0L) {                              /* Check for EINTR          */
      if(errno == EINTR) {                         /* Yes, Continue processing */
        continue;                                  /*                          */
      }                                            /*                          */
      break;                                       /*                          */
    }                                              /*                          */

    /*-------------------------------------------------------------------------*/
    /* One or more unix fd's have read data or an exception                    */
    /*-------------------------------------------------------------------------*/

    else if(status > 0) {                          /* One more fd have data    */

      /*.......................................................................*/
      /* Process any possible standard read or exceptions indicated by select  */
      /*.......................................................................*/

      if(UnixFd >= 0L) {                           /* Data for broker ?        */
        status = select_tstfd(&ex_set,UnixFd);     /* Is there an exception    */
        if(status != 0L) {
          status = -1L;
        }
        else {
          status = select_tstfd(&rd_set,UnixFd);   /* Any actual data          */
          if(status != 0) {                        /* Yes, go and get it       */
            status = read(UnixFd,bytes,length);    /* Go and read characters   */
            if(status == 0) {
               status = -1;
            }
            break;                                 /* Process it               */
          }                                        /*                          */
        }
      }                                            /*                          */
    }                                              /*                          */

    /*-------------------------------------------------------------------------*/
    /* The select timed out - do timeout processing                            */
    /*-------------------------------------------------------------------------*/

    else {                                         /* Select timed out         */

      curr_time = time(NULL);                      /* Get current time         */

    }                                              /*                          */
  }                                                /*                          */

  /*===========================================================================*/
  /* End of the for(;;) loop                                                   */
  /*===========================================================================*/

  return(status);                                  /* Return tty/tcp data info */

}                                                  /*                          */

