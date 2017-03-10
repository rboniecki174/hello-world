/************************************************************************************/
/*                                                                                  */
/* switch_logfile.c                                                       R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains a function that is used to switch the log file that is        */
/* attached to stderr and stdout.                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* The name of the logfile that will be generated for a prefix of "GARBAN" would    */
/* look like                                                                        */
/*                                                                                  */
/*            GARBAN-19991202-000002-Dec-Thu.LOG                                    */
/*                                                                                  */
/* given the date of 02/Dec/1999 at 00:00:02 in the morning.  By using this format  */
/* the name of the log files will be unique to the second eliminating the need to   */
/* check if a log file of a given sequence number exists.  The abbriviated day and  */
/* month names are added to aid in readability of the filenames.                    */
/*                                                                                  */
/* The naming format allows ls to put the names in ascending order.                 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        01/Dec/1999  File created                                */
/*                                                                                  */
/************************************************************************************/


#define _POSIX4_DRAFT_SOURCE

/*==================================================================================*/
/* System Include Files                                                             */
/*==================================================================================*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

/*==================================================================================*/
/* Local Include Files and Data Definitions                                         */
/*==================================================================================*/

#define PREFIX_LEN         255L            /* Maximum Length of the Prefix string   */
#define DATETIME_LEN       255L            /* Maximum Length of the Date/Time string*/
#define LOGNAME_LEN        255L            /* Maximum Length of the Extra space     */
#define FORMAT       "%Y%m%d-%H%M%S-%b-%a" /* Date and Time format                  */

#define FD_STDERR     0x80000000
#define FD_STDOUT     0x40000000


/*==================================================================================*/
/* Main "C" Interface                                                               */
/*==================================================================================*/

int             switch_logfile(unsigned char * prefix, unsigned int        flag)
{
  int                  rtnstatus;
  int                  status;
  int                  index; 
  int                  wlength;
  unsigned char        wprefix[PREFIX_LEN+1L];
  unsigned char        datetime[DATETIME_LEN+1L];
  unsigned char        logname[PREFIX_LEN+DATETIME_LEN+LOGNAME_LEN+1L];
  size_t               dlength;
  time_t               curr_time;
  struct tm          * adr_result;
  FILE               * file;

  /*--------------------------------------------------------------------------------*/
  /* Process the arguments and prepare the prefix for use                           */
  /*--------------------------------------------------------------------------------*/

  rtnstatus = 0L;                                /* Clear the return status         */

  wlength   = strlen((char *)prefix);                    /* Get the prefix length           */
  if(wlength > PREFIX_LEN) {                     /* To int  ?                       */
    wlength = PREFIX_LEN;                        /* Yes, Use Maximum                */
  }                                              /*                                 */

  memset(wprefix,0x00,  PREFIX_LEN+1L);          /* Clear the work space            */
  memcpy(wprefix,prefix,wlength);                /* Move the prefix                 */

  for(index=wlength-1L; index>0; index--) {      /* Remove trailing blanks          */
    if(wprefix[index] != ' ') {                  /* Not a trailing blank            */
      break;                                     /* Exit this loop                  */
    }                                            /*                                 */
    wprefix[index] = '\0';                       /* Make this a null                */
  }                                              /*                                 */

  wlength = strlen((char *)wprefix);                     /* Regenerate the length           */
  for(index=0L; index<wlength; index++) {        /* Convert embedded blanks to '_'  */
    if(wprefix[index] == ' ') {                  /* Not a trailing blank            */
      wprefix[index] = '_';                      /* Convert the embedded blank      */
    }                                            /*                                 */
  }                                              /*                                 */

  /*--------------------------------------------------------------------------------*/
  /* Generate the date and time string                                              */
  /*--------------------------------------------------------------------------------*/

  curr_time  = time((time_t *) NULL);            /* Get the current time            */
#if 0
  adr_result = (struct tm *) localtime_r(&curr_time,&result);  /* Decode the time   */
#else
  adr_result = localtime(&curr_time);
#endif
  dlength    = strftime((char *)datetime,DATETIME_LEN,FORMAT,adr_result);

  /*--------------------------------------------------------------------------------*/
  /* Build the log file name                                                        */
  /*--------------------------------------------------------------------------------*/
  
  memset(logname,0x00,sizeof(logname));          /* Zero out the logname space      */

  (void) strcat((char *)logname,(char *)wprefix);                /* Insert the prefix               */
  (void) strcat((char *)logname,"-");                    /* Add a dash for readability      */
  (void) strcat((char *)logname,(char *)datetime);               /* Add the date and time of the log*/
  (void) strcat((char *)logname,".LOG");                 /* Add the string ".LOG"           */
#ifdef MAIN
  fprintf(stdout,"Log File Name : <%s>\n",logname);
#endif

  /*--------------------------------------------------------------------------------*/
  /* Go and Process the request                                                     */
  /*--------------------------------------------------------------------------------*/

  if((flag & FD_STDERR) == FD_STDERR) {
    status = fflush(stderr);
    status = fclose(stderr);
    file   = freopen((char *)logname,"a+",stderr);
  }

  if((flag & FD_STDOUT) == FD_STDOUT) {
    status = fflush(stdout);
    status = fclose(stdout);
    file   = freopen((char *)logname,"a+",stdout);
  }

  /*--------------------------------------------------------------------------------*/
  /* Return to the caller                                                           */
  /*--------------------------------------------------------------------------------*/

  return(rtnstatus);                             /* Return to the caller            */

}                                                /*                                 */


/*==================================================================================*/
/* Main FORTRAN Interface                                                           */
/*==================================================================================*/

void switch_logfile_(unsigned char      * prefix,     unsigned int      * flag, 
                     unsigned int       * rtnstatus,  int                  prefix_len)
{
  int                  wlength;
  unsigned char        wprefix[PREFIX_LEN+1L];

  /*--------------------------------------------------------------------------------*/
  /* Process the arguments and prepare the prefix for use                           */
  /*--------------------------------------------------------------------------------*/

  wlength   = prefix_len;                        /* Get the prefix length           */
  if(wlength > PREFIX_LEN) {                     /* To int  ?                       */
    wlength = PREFIX_LEN;                        /* Yes, Use Maximum                */
  }                                              /*                                 */

  memset(wprefix,0x00,  PREFIX_LEN+1L);          /* Clear the work space            */
  memcpy(wprefix,prefix,wlength);                /* Move the prefix                 */

  /*--------------------------------------------------------------------------------*/
  /* Go and Process the request                                                     */
  /*--------------------------------------------------------------------------------*/

  (*rtnstatus) = switch_logfile(wprefix,(*flag));/* Switch the log file itself      */

  return;                                        /* Return to the caller            */
}                                                /*                                 */


/*==================================================================================*/
/* Test Program                                                                     */
/*==================================================================================*/

#ifdef MAIN

int      main(int      argc, unsigned char ** argv)
{
  int                  flag;
  int                  prefix_len;
  unsigned char      * prefix;
  int                  rtnstatus;

  if(argc < 2L) {
    fprintf(stderr,"A Prefix is required as the first argument.\n");
    exit(1L);
  }
  
  prefix       = argv[1L];                       /* Get the prefix value            */
  prefix_len   = strlen(prefix);                 /* Length of the prefix            */
  flag         = (FD_STDERR | FD_STDOUT);        /* Set flag                        */
  
  (void)         switch_logfile_(prefix,&flag,&rtnstatus,prefix_len);

  fprintf(stderr,"This I/O is to stderr\n");
  fprintf(stdout,"This I/O is to stdout\n");

  exit(0L);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
