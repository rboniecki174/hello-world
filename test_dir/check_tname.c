/************************************************************************************/
/*                                                                                  */
/* check_tname.c   Check a Task Name for correctness                      R00-00.00 */
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
/* tname     - This is the address of the string that is to be checked.             */
/* tname_len - This is the length of the string to check.                           */
/*                                                                                  */
/* The returned values of this routine are:                                         */
/*                                                                                  */
/*    0 - The name is valid.                                                        */
/*    1 - The name does not start with a alphabetic character.                      */
/*    2 - The name contains invalid data.                                           */
/*    3 - The name is more than 8 non-blank characters in length.                   */
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
/* R00-00.00  DRLJr        30/Mar/1999  Routine moved to its own file.              */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define       TASKNAMELEN            8



/*==================================================================================*/
/* check_tname                                                                      */
/*==================================================================================*/

int      check_tname_(char * tname, int      tname_len)
{
  int            rtnstat;                        /* Return status                   */
  int            index;                          /* General purpose index           */
  int            status;                         /* General purpose status variable */

  rtnstat  = 0L;                                 /* Assume name is good             */

  if(tname_len <= 0L) {                          /* No length to the name           */
    rtnstat = 1L;                                /* Indicate invalid name           */
    goto exit_here;                              /* Return to the caller            */
  }                                              /*                                 */

  status = isalpha((int     ) tname[0]);         /* First character alphabetic      */
  if(status == 0) {                              /* No it is not                    */
    rtnstat = 1L;                                /* Indicate invalid name           */
    goto exit_here;                              /* Exit from the routine           */
  }                                              /*                                 */

  for(index=1;index<tname_len;index++) {         /* Check each character            */
    status = isalnum((int     ) tname[index]);   /* Is character alphanumeric       */
    if(status == 0L) {                           /* Not alphnumeric                 */
      break;                                     /* Exit this loop                  */
    }                                            /*                                 */
    if(index >= TASKNAMELEN) {                   /* Name is too int                 */
      rtnstat = 3L;                              /* Indicate too int                */
      goto exit_here;                            /* Go and exit                     */
    }                                            /*                                 */
  }                                              /*                                 */

  for(;index<tname_len;index++) {                /* All remaining characters == ' ' */
    if(tname[index] != ' ') {                    /* Not a blank character           */
      rtnstat = 2L;                              /* Indicate invalid                */
      goto exit_here;                            /* Go and exit processing          */
    }                                            /*                                 */
  }                                              /*                                 */

  exit_here:                                     /* Exit point of the routine       */

  return(rtnstat);                               /* Return the caller               */
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
