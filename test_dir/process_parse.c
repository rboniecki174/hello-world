/************************************************************************************/
/*                                                                                  */
/* process_parse.c    RUN-FILE support for parsing                        R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines that are used to support the OS/32 sub-system    */
/* and the RUN-FILE assignment mechanism.                                           */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   Various based upon the arguments of the functions.                             */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* The "C" version of each function/subroutine is first and the FORTRAN interface   */
/* function/subroutine is second.  The FORTRAN interface function/subroutine        */
/* processes the arguments into a "C" function call.                                */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        11/Mar/1999  New file created from process_asn.f         */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include Files                                                                    */
/*==================================================================================*/

#include <stdio.h>
#include <ctype.h>

#include "private_data.h"
int      lnblnk_(unsigned char * buffer, int        length);

/*==================================================================================*/
/* Special Macros                                                                   */
/*==================================================================================*/

#define BLANK_OR_TAB(x)      ((x == ' ') || (x == '\t'))
#define NOT_BLANK_TAB(x)     ((x != ' ') && (x != '\t'))

/*==================================================================================*/
/* Static Data Definitions                                                          */
/*==================================================================================*/


/*==================================================================================*/
/* FBLNK (Skip to First/Next Blank - allow tabs also)                               */
/*==================================================================================*/

int      fblnk(unsigned char * buffer, int        index, int        length)
{
  int         indx;                        /* Working indx value to return          */

  if(index >= length) {                    /* At or past end of string ?            */
    indx = index;                          /* Yes, return the current index value   */
  }                                        /*                                       */
  else {                                   /* No, look for the blank                */
    for(indx=index;indx<length;indx++) {   /* Look from index to end of buffer      */
      if(BLANK_OR_TAB(buffer[indx])) {     /* On the first or next blank ?          */
        break;                             /* Yes, exit processing loop             */
      }                                    /*                                       */
    }                                      /*                                       */
  }                                        /*                                       */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */

int      fblnk_(unsigned char * buffer, int      * index, int        length)
{
  int           indx;                      /* Value to return to caller             */
  int           jndx;      

  jndx        = (*index) - 1L;             /* Convert to "C" based offsets          */
  indx        = fblnk(buffer,jndx,length); /* Locate the blank                      */
  indx++;                                  /* Convert to FORTRAN based offsets      */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */

/*==================================================================================*/
/* FNBLNK (Skip to First/Next Non-Blank - allow tabs also)                          */
/*==================================================================================*/

int      fnblnk(unsigned char * buffer, int        index, int        length)
{
  int         indx;                        /* Working indx value to return          */

  if(index >= length) {                    /* At or past end of string ?            */
    indx = index;                          /* Yes, return the current index value   */
  }                                        /*                                       */
  else {                                   /* No, look for the blank                */
    for(indx=index;indx<length;indx++) {   /* Look from index to end of buffer      */
      if(NOT_BLANK_TAB(buffer[indx])) {    /* Not on the first or next non-blank ?  */
        break;                             /* Yes, exit processing loop             */
      }                                    /*                                       */
    }                                      /*                                       */
  }                                        /*                                       */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */

int      fnblnk_(unsigned char * buffer, int      * index, int        length)
{
  int           indx;                      /* Value to return to caller             */
  int           jndx;                      /* "C" value of index                    */

  jndx        = (*index) - 1L;             /* Convert to "C" based offsets          */
  indx        = fnblnk(buffer,jndx,length);/* Locate the blank                      */
  indx++;                                  /* Convert to FORTRAN based offsets      */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */


/*==================================================================================*/
/* LNBLNK (Skip to Last Non-Blank Character (allow tabs also) - Replaced FTN RTL    */
/*==================================================================================*/

int      lnblnk(unsigned char * buffer, int        length)
{
  int         indx;                        /* Working indx value to return          */

  for(indx=length-1L;indx>=0L;indx--) {    /* Look from end to start of buffer      */
    if((NOT_BLANK_TAB(buffer[indx]))) {    /* On the first or next blank ?          */
      break;                               /* Yes, exit processing loop             */
    }                                      /*                                       */
  }                                        /*                                       */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */

int      lnblnk_(unsigned char * buffer, int        length)
{
  int           indx;                      /* Value to return to caller             */

  indx        = lnblnk(buffer,length);     /* Locate the non-blank                  */
  indx++;                                  /* Convert to FORTRAN based offsets      */
  return(indx);                            /* Return the current index              */
}                                          /*                                       */


/*==================================================================================*/
/* GETVALUE (Get and Decode a Decimal Numeric String)                               */
/*==================================================================================*/

#define  ADD(x)   ((value * 10L) + x)

int      getvalue(unsigned char * buffer, int        index, int        jndex)
{
  int             value;                   /* Value that is returned to the caller  */
  int             indx;                    /* Current character index               */
  int             byte;                    /* Byte that is being processed          */
  int             count;                   /* Number of bytes converted             */

  if(jndex < index) {                      /* Something wrong                       */
    value = -1L;                           /* Indicate a problem                    */
    return(value);                         /* Exit now                              */
  }                                        /*                                       */

  count   = 0L;                            /* Number of bytes converted             */
  value   = 0L;                            /* Set return value to 0                 */
  for(indx=index;indx<=jndex;indx++) {     /* Check each possble character          */
    byte = (int     ) buffer[indx];        /* Get the byte as an integer            */
    if(isdigit(byte)) {                    /* Is a decimal digit                    */
      switch(byte) {                       /* Select the value to add to value      */
        case '0' :  value = ADD(0L); break;/* Add in a zero value                   */
        case '1' :  value = ADD(1L); break;/* Add in a value of 1                   */
        case '2' :  value = ADD(2L); break;/* Add in a value of 2                   */
        case '3' :  value = ADD(3L); break;/* Add in a value of 3                   */
        case '4' :  value = ADD(4L); break;/* Add in a value of 4                   */
        case '5' :  value = ADD(5L); break;/* Add in a value of 5                   */
        case '6' :  value = ADD(6L); break;/* Add in a value of 6                   */
        case '7' :  value = ADD(7L); break;/* Add in a value of 7                   */
        case '8' :  value = ADD(8L); break;/* Add in a value of 8                   */
        case '9' :  value = ADD(9L); break;/* Add in a value of 9                   */
      }                                    /* End of switch()                       */
      count++;                             /* Count the byte converted              */
    }                                      /*                                       */
    else {                                 /* Byte is not a decimal digit           */
      value = -1L;                         /* Stop Processing and report problem    */
      break;                               /* Exit Processing                       */
    }                                      /*                                       */
  }                                        /*                                       */
  if(count <= 0L) {                        /* No digits converted                   */
    value = -1L;                           /* Stop Processing and report problem    */
  }                                        /*                                       */
  return(value);                           /* Return value to caller                */
}                                          /*                                       */

int      getvalue_(unsigned char * buffer, int      * index, int      * jndex)
{
  int             value;                   /* Value that is returned to the caller  */
  int             indexc;                  /* "C" Index Value                       */
  int             jndexc;                  /* "C" Index Value                       */

  indexc = (*index) - 1L;                  /* Convert to "C" or zero based indexing */
  jndexc = (*jndex) - 1L;                  /* Convert to "C" or zero based indexing */
  value  = getvalue(buffer,indexc,jndexc); /* Call the "C" routine                  */
  return(value);                           /* Return to the caller                  */
}                                          /*                                       */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
