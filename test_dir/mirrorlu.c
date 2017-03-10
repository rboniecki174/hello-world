/********************************************************************/
/*                                                                  */
/* mirrorlu.c     Open the Mirror File for a file         R00-01.01 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* The path name and the filename are expected to be passed from    */
/* >>> FORTRAN << without any trailing spaces and from >>> "C" <<<  */
/* without any trailing spaces or the NULL bytes indicated.         */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  06/Mar/1999  DRLJr     New Function                   */
/* R00-01.00  12/Mar/1999  DRLJr     Added function mirror_close()  */
/* R00-01.01  17/Mar/1999  DRLJr     Added the underscore to the    */
/*                                   mirror_close() functiun so     */
/*                                   FORTRAN can call the routine.  */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <string.h>
#include <dlogdefs.h>


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "proto_common.h"

#define  MODE (O_RDWR)            

static char fmt1[] =
   {" mirrorlu.c open failure : Errno %5d (%s)" };
static char fmt2[] =
   {" mirrorlu.c open failure : Mirror File=<%s>" };

/*==================================================================*/
/* Program Code for mirrorlu_()                                     */
/*==================================================================*/

int      mirrorlu_(unsigned char * mirrpath, unsigned char * filepath,
                   int             mirrlen,  int             filelen)
{
  int          fd;                        /* Fd lu is assigned      */
  int          mvlen;                     /* Length of move data    */
  int          index;                     /* Working index          */
  char         cfile[1024];               /* Work copy of the file  */
  char         ffile[1024];               /* Work copy              */
  char         pfile[1024];               /* Work copy              */
  char       * errstr;                    /* Error string pointer   */

  /*================================================================*/
  /* Zero the work space prior to moving the path and file names    */
  /*================================================================*/

  memset(cfile,0x00,1024);                /* Zero the buffer        */
  memset(ffile,0x00,1024);                /* Zero the buffer        */
  memset(pfile,0x00,1024);                /* Zero the buffer        */
  index  =     0L;                        /* Set work index to 0    */

  /*================================================================*/
  /* Move the pathname into the work space                          */
  /*================================================================*/

  mvlen     =  mirrlen;                   /* Length of data to move */
  if(mvlen > 512-2) {                     /* Limit path name length */
    mvlen = 512-2;                        /* Leave space for a null */
  }                                       /* and a slash            */
  memcpy(&(cfile[index]),mirrpath,mvlen); /* Move the path name     */
  memcpy(&(pfile[    0]),mirrpath,mvlen); /* Move the path name     */
  index     = index + mvlen;              /* Location of next byte  */

  /*================================================================*/
  /* Move the directory slash into the work space                   */
  /*================================================================*/

  cfile[index] = '/';                     /* Insert the slash       */
  index++;                                /* Point to next location */

  /*================================================================*/
  /* Move the filename into the work space                          */
  /*================================================================*/

  mvlen     =  filelen;                   /* Length of data to move */
  if(mvlen > 512-2) {                     /* Limit path name length */
    mvlen = 512-2;                        /* Leave space for a null */
  }                                       /* and a slash            */
  memcpy(&(cfile[index]),filepath,mvlen); /* Move the path name     */
  memcpy(&(ffile[    0]),filepath,mvlen); /* Move the path name     */
  index     = index + mvlen;              /* Location of next byte  */

  /*================================================================*/
  /* Open the  mirror file                                          */
  /*================================================================*/

  fd        = open(cfile,MODE);           /* Open the mirror file   */
  dLog(DLOG_MINOR,
    " (mirrorlu)  : Mirrored File  %-30s fd %3d (path %s).\n",
                 ffile,fd,pfile);      
  if(fd < 0L) {                           /* Error                  */
    errstr = strerror(errno);             /* Get the error code     */
    dLog(DLOG_MAJOR,fmt1,errno,errstr);    /* Write error message    */
    dLog(DLOG_MAJOR,fmt2,cfile);           /* Write error message    */
  }                                       /*                        */

  return(fd);                             /* Return to the caller   */

}                                         /*                        */

/*==================================================================*/
/* Close a mirror fd for a lu                                       */
/*==================================================================*/

void mirror_close_(int      * lu)
{
  int             fd;
  int             status;

  (void) get_mirr_fd_(lu,&fd);
  if(fd >= 0) {
    status = close(fd);
  }

  return;
}

/********************************************************************/
/*                                                                  */
/********************************************************************/
