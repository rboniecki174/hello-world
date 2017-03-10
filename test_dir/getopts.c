/************************************************************************************/
/*                                                                                  */
/* getopts.c        Collect Options and Make into a single string         R00-01.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   No arguments                                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* This routine is a FORTRAN only program.  It calls several FORTRAN routines.      */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        06/Mar/1999  Created "C" version of common block.        */
/* R00-00.01  DRLJr        21/May/1999  Return the string length                    */
/* R00-01.00  DRLJr        27/Mar/2000  Added routine getargn_()                    */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include files used                                                               */
/*==================================================================================*/

#include <stdio.h>
#include <string.h>

#include "private_data.h"

/*==================================================================================*/
/* MACRO Defintions                                                                 */
/*==================================================================================*/

#define UCHAR    unsigned char 
#define WRKSIZE  512

/*==================================================================================*/
/* Prototypes for FORTRAN Functions called                                          */
/*==================================================================================*/

int        iargc_(void);
void       getarg_(int      * argno, UCHAR * arg, int      arglen);
void       getargn_(int      * argno, UCHAR * arg, int      arglen);

/*==================================================================================*/
/* Prototypes for INTERNAL Functions                                                */
/*==================================================================================*/

static int       lnblnk(UCHAR * buffer, int       length);/* Internal Support Fnc   */

/*==================================================================================*/
/* getopts() Subroutine                                                             */
/*==================================================================================*/

void getopts_(UCHAR * args, int      * length, int      args_len)
{
  int            nargs;                             /* Number of FORTRAN arguments  */
  int            cargs;                             /* Current Argument             */
  int            sargs;                             /* Stating Argument             */
  int            offset;                            /* Offset into buffer           */
  int            argend;                            /* Last non-blank in argbuf     */
  int            argstr;                            /* Debug variable               */
  UCHAR          buffer[WRKSIZE];                   /* Working Buffer               */
  UCHAR          argbuf[WRKSIZE];                   /* Working Buffer               */
  
  memset(argbuf,(int     ) ' ',WRKSIZE);            /* Clear buffer to blanks       */
  sargs = private_data.first_task_arg;              /* Get the starting argument #  */
  if(sargs <= 0L) {                                 /* Is not currently set         */
    sargs = 1L;                                     /* Set to 1 - FORTRAN based     */
  }                                                 /*                              */

  nargs   = iargc_();                               /* Get the number of arguments  */
  argend  = 0L;                                     /* Start at position 0          */
  argstr  = 0L;                                     /* Start at position 0          */

  for(cargs=sargs;cargs<=nargs;cargs++) {           /* Process each argument        */

    argstr  = argend;                               /* Save the current offset      */

    if(cargs != sargs) {                            /* Add a comma to the string    */
      argbuf[argend] = ',';                         /* Add a comma                  */
      argend++;                                     /* Point to the next position   */
    }                                               /*                              */

    (void) getarg_(&cargs,buffer,WRKSIZE);          /* Get the argument             */
    offset = lnblnk(buffer,WRKSIZE);                /* Get the basic length         */
    if(argend + offset >= WRKSIZE) {                /* We are done                  */
      break;                                        /*                              */
    }                                               /*                              */
    if(offset >= 0L) {                              /*                              */
      memcpy(&argbuf[argend],buffer,offset+1L);     /* Add the new argument         */
    }                                               /*                              */
    argend = argend + offset + 1L;                  /* Offset to the end            */

  }                                                 /*                              */

  memset(args,(int     ) ' ',args_len);             /* Blank the callers buffer     */
  offset  = (args_len < WRKSIZE) ? args_len : WRKSIZE; /* Determine the maximum len */
  memcpy(args,argbuf,offset);                       /* Return the user's buffer     */

  (*length) = argend;                               /* Return the string length     */

  return;                                           /*                              */
}                                                   /*                              */

/*==================================================================================*/
/* getargn - Special Interface routine to EPC FORTRAN getarg_() routine             */
/*==================================================================================*/

void       getargn_(int      * argno, UCHAR * arg, int      arglen)
{
  int          argn;

  if((*argno) >  0L) {                              /* Task Relative Arguments      */
    argn  = (*argno) + private_data.first_task_arg; /* Get the starting argument #  */
    argn--;                                         /* And adjust value             */
  }                                                 /*                              */
  else if((*argno) == 0L) {                         /* Get task name etc.           */
    argn  = 0L;                                     /* Get the task name            */
  }                                                 /*                              */
  else {                                            /* Get Run File Name            */
    argn  = 1L;                                     /* Get the Run File Name        */
  }                                                 /*                              */
  (void)   getarg_(&argn,arg,arglen);               /* Get the argument             */
  return;                                           /* Return to the caller         */
}

/*==================================================================================*/
/* LNBLNK (Skip to Last Non-Blank Character (allow tabs also) - Replaced FTN RTL    */
/*==================================================================================*/

static int      lnblnk(UCHAR * buffer, int        length)
{
  int         indx;                        /* Working indx value to return          */

  for(indx=length-1L;indx>=0L;indx--) {    /* Look from end to start of buffer      */
    if(buffer[indx] != ' ') {              /* On the first or next blank ?          */
      break;                               /* Yes, exit processing loop             */
    }                                      /*                                       */
  }                                        /*                                       */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */

/*==================================================================================*/
/* MAIN Test Program                                                                */
/*==================================================================================*/

#ifdef MAIN

static int          xargc;
static UCHAR     ** xargv;

int      main(int      argc, UCHAR ** argv)
{
  int         index;
  UCHAR       wrkbuffer[80L+1L];

  if(argc < 2L) {
    xargc    =  0L;
  }
  else {
    xargc    =  argc - 1L;
    xargv    =  argv;
  }
  wrkbuffer[80L] = '\0';

  for(index=1;index<argc;index++) {
    fprintf(stderr," %s ",argv[index]);
  }

  index   =  80L;
  getopts_(wrkbuffer,&index,index);
  fprintf(stderr,"\n");
  fprintf(stderr,"%s\n",wrkbuffer);
  fprintf(stderr,"length is %d\n",index);

  exit(0L);
}

void getarg_(int      * argno, UCHAR * arg, int      arglen)
{
  int          length;
  int          mvlen;

  memset(arg,(int     ) ' ',arglen);
  length = strlen(xargv[(*argno)]);
  mvlen  = (arglen < length) ? arglen : length;
  memcpy(arg,xargv[(*argno)],mvlen);
  return;

}

int      iargc_(void)
{
  return(xargc);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
