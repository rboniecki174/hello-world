/*********************************************************************************/
/*                                                                               */
/* select_support.c    Specialized support routines for select()       R00-02.00 */
/*                                                                               */
/*===============================================================================*/
/*                                                                               */
/* Purpose:                                                                      */
/*                                                                               */
/*===============================================================================*/
/*                                                                               */
/* Revision:                                                                     */
/*                                                                               */
/* Revision  Date        Who   Why                                               */
/* ========= =========== ===== ==================================================*/
/* R00-00.00 ??/???/???? DRLJr Initial version                                   */
/* R00-01.00 04/Mar/1999 DRLJr Added select_wait routine                         */
/* R00-02.00 22/Apr/1999 DRLJr Added the select_exec front-end routine to the    */
/*                             select_wait routine.  The select_exec routine     */
/*                             uses a master fd set for all three sets.  If the  */
/*                             master fd set is NULL then it will skip the copy  */
/*                             operation of the master set to the read, write    */
/*                             and exception sets.                               */
/*                                                                               */
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ulimit.h>

/*===============================================================================*/
/*                                                                               */
/* select_maxfd   Get the maximum number of fds to support                       */
/*                                                                               */
/*===============================================================================*/

int      select_maxfd()
{
  int           maxfd;                     /* Maximum number of fds to support   */

/*maxfd  =  ulimit(GET_MAX_OPEN,NULL);*/   /* Get the maximum number of fds      */
  maxfd  =  sysconf(_SC_STREAM_MAX);       /* Get the maximum number of fds      */
  if(maxfd < FOPEN_MAX) {                  /* Support at least FOPEN_MAX (Note:  */
    maxfd = FOPEN_MAX;                     /* this allows for errors also).      */
  }                                        /*                                    */
  if(maxfd > FD_SETSIZE) {                 /* To large for the standard setting? */
    maxfd = FD_SETSIZE;                    /* Yes, use the smaller size          */
  }                                        /*                                    */
  return(maxfd);                           /* Return this value to the caller    */
}                                          /*                                    */

/*===============================================================================*/
/*                                                                               */
/* select_alloc   Set-up a data area for the select function                     */
/*                                                                               */
/*===============================================================================*/

fd_set * select_alloc()
{
  fd_set     * fdset;                      /* The address of the FD set          */

  fdset  = (fd_set *) malloc(sizeof(fd_set)); /* Allocate the map                */
  if(fdset == (fd_set *) NULL) {           /* Malloc failed - report to caller   */
    return((fd_set *) NULL);               /* Return NULL address                */
  }                                        /*                                    */

  FD_ZERO(fdset);                          /* Clear the fd set                   */

  return(fdset);                           /* Return address to caller           */
}                                          /*                                    */


/*===============================================================================*/
/*                                                                               */
/* select_zerofd  Zero an fdset                                                  */
/*                                                                               */
/*===============================================================================*/

int      select_zerofd(fd_set * fdset)
{
  int       status;                        /* General purpose status variable    */

  status = 0L;                             /* Set the status to zero             */
  FD_ZERO(fdset);                          /* Zero the fd set                    */
  return(status);                          /* Return to the caller               */
}                                          /*                                    */

/*===============================================================================*/
/*                                                                               */
/* select_setfd  Set a unix fd for testing into an fdset                         */
/*                                                                               */
/*===============================================================================*/

int      select_setfd(fd_set * fdset, int      fd)
{
  int       status;                        /* General purpose status variable    */
  int       maxfd;                         /* Maximum possible fd value          */

  maxfd  = select_maxfd();                 /* Get the maximum possible fd        */
  status = 0L;                             /* Set the status to zero             */

  if((fd < 0) || (fd >= maxfd)) {          /* Check for range error              */
    status = -1L;                          /* Indicate an error                  */
  }                                        /*                                    */
  else {                                   /* Set the particular fd              */
    FD_SET(fd,fdset);                      /* Do the selection                   */
  }                                        /*                                    */
  return(status);                          /* Return to the caller               */
}                                          /*                                    */

/*===============================================================================*/
/*                                                                               */
/* select_tstfd  Test a unix fd for testing in an fdset                          */
/*                                                                               */
/* Note: FD_ISSET() returns either a zero or the value based on the bit location */
/*       location being tested.  The returned value can be postive, negative or  */
/*       0.  To check for the error condition a explicit check for the -1 must   */
/*       be performed.      DRLJr 22/Dec/1997                                    */
/*                                                                               */
/*===============================================================================*/

int      select_tstfd(fd_set * fdset, int      fd)
{
  int       status;                        /* General purpose status variable    */
  int       maxfd;                         /* Maximum possible fd value          */

  maxfd  = select_maxfd();                 /* Get the maximum possible fd        */
  status = 0L;                             /* Set the status to zero             */

  if((fd < 0) || (fd >= maxfd)) {          /* Check for range error              */
    status = -1L;                          /* Indicate an error                  */
  }                                        /*                                    */
  else {                                   /* Set the particular fd              */
    status = FD_ISSET(fd,fdset);           /* Do the selection                   */
  }                                        /*                                    */
  return(status);                          /* Return to the caller               */
}                                          /*                                    */


/*===============================================================================*/
/*                                                                               */
/* select_clrfd  Clear a unix fd for testing into an fdset                       */
/*                                                                               */
/*===============================================================================*/

int      select_clrfd(fd_set * fdset, int      fd)
{
  int       status;                        /* General purpose status variable    */
  int       maxfd;                         /* Maximum possible fd value          */

  maxfd  = select_maxfd();                 /* Get the maximum possible fd        */
  status = 0L;                             /* Set the status to zero             */

  if((fd < 0) || (fd >= maxfd)) {          /* Check for range error              */
    status = -1L;                          /* Indicate an error                  */
  }                                        /*                                    */
  else {                                   /* Clear the particular fd            */
    FD_CLR(fd,fdset);                      /* Do the selection                   */
  }                                        /*                                    */
  return(status);                          /* Return to the caller               */
}                                          /*                                    */

/*===============================================================================*/
/*                                                                               */
/* select_wait  Wait for a fd to become ready for reading, writing or exception  */
/*                                                                               */
/*===============================================================================*/

int      select_wait(int      maxfd, fd_set * rset, fd_set * wset, fd_set * eset,
                     int      tmout)
{
  int              status;                     /* Status of the select           */
  int              secs;                       /* Number of seconds              */
  int              msecs;                      /* Number of milliseconds         */
  struct timeval   tout;                       /* Select Timeout structure       */
  struct timeval * a_tout;                     /* Address of tout                */


  if(tmout <  0L) {
    a_tout = (struct timeval *) NULL;          /* Wait forever                   */
  }                                            /*                                */
  else if(tmout == 0L) {                       /* Check if data is present       */
    a_tout       = &tout;                      /* Set the address                */
    tout.tv_sec  = 0L;                         /* Set the timeout                */
    tout.tv_usec = 0L;                         /* Set the timeout                */
  }                                            /*                                */
  else {                                       /* Wait for some time             */
    a_tout       = &tout;                      /* Set the address                */
    secs         = tmout / 1000;               /* Number of seconds              */
    msecs        = tmout % 1000;               /* Number of milliseconds         */
    tout.tv_sec  = secs;                       /* Set the timeout                */
    tout.tv_usec = msecs * 1000;               /* Set the timeout                */
  }                                            /*                                */
    
  status = select(maxfd,rset,wset,eset,a_tout);/* Wait for the event             */
  if(status < 0L) {                            /* Error detected                 */
    status = errno * (-1L);                    /* Return a negative errno value  */
  }                                            /*                                */

  return(status);                              /* Return to the caller           */

}                                              /*                                */


/*===============================================================================*/
/*                                                                               */
/* select_exec  Execute a select() using the specified master select fd list     */
/*                                                                               */
/*===============================================================================*/

int      select_exec(int        maxfd, 
                     fd_set   * mset, fd_set * rset, fd_set * wset, fd_set * eset,
                     int        tmout)
{
  int          status;                               /* Returned status value    */

  if(mset != (fd_set *) NULL) {                      /* A master set is present  */
    if(rset != (fd_set *) NULL) {                    /* Read Set Present         */
      memcpy(rset,mset,sizeof(fd_set));              /* Copy master to set       */
    }                                                /*                          */
    if(wset != (fd_set *) NULL) {                    /* Write Set Present        */
      memcpy(wset,mset,sizeof(fd_set));              /* Copy master to set       */
    }                                                /*                          */
    if(eset != (fd_set *) NULL) {                    /* Exception set present    */
      memcpy(eset,mset,sizeof(fd_set));              /* Copy master to set       */
    }                                                /*                          */
  }                                                  /*                          */
  status  = select_wait(maxfd,rset,wset,eset,tmout); /* Go and do the wait       */

  return(status);                                    /* Return to the caller     */
}                                                    /*                          */

/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/

