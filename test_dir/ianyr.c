/************************************************************************************/
/*                                                                                  */
/* ianyr.c   Find any character in second string in first string (right)  R00-00.01 */
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
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        16/Apr/1999  Convert any.f to "C" (ianyr.c)              */
/* R00-00.01  DRLJr        17/May/1999  Fixed problems.                             */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>

int      ianyr_(unsigned char * src,    unsigned char * chr,
                int             srclen, int             chrlen)
{
  int          sindx;                       /* Index into source string             */
  int          cindx;                       /* Index into character string          */
  int          index;                       /* Value to return to caller            */

  index    = 0L;                            /* Indicate byte is not found           */

  for(sindx=srclen-1;sindx>=0;sindx--) {    /* Walk through the source string       */
    for(cindx=0;cindx<chrlen;cindx++) {     /* Walk through the character string    */
      if(src[sindx] == chr[cindx]) {        /* Are these the same character         */
        index = sindx + 1;                  /* Yes, Return 1 based index            */
        break;                              /* Exit to caller                       */
      }                                     /*                                      */
    }                                       /*                                      */
    if(index != 0L) {
      break;
    }
  }                                         /*                                      */
  return(index);                            /* Return to the caller                 */
}                                           /*                                      */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
