/************************************************************************************/
/*                                                                                  */
/* peekt.c      FORTRAN Support Utilities                                 R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains "C" definition of the routines that were located in the file  */
/* peekt.f.                                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/* See individual routines                                                          */
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
/* R00-00.00  DRLJr        25/Mar/1998  File created to replace peekt.f             */
/* R01-00.00 jeffsun       21/Feb/2001  Remove redundant externs                    */
// tt60135    GJ           02/Mar/2011  Replace obsolete realtime1.h with gb1_init.h
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "gb1_init.h"
#include "private_data.h"
void get_tname_(char * tname, int      tname_len);

#if 0
extern   cmn_realtime1     realtime1;            /* Reference to the common block   */
extern   cmn_private_data  private_data;         /* Reference to the common block   */
#endif



/*==================================================================================*/
/*                                                                                  */
/* peekt()                                                                          */
/*                                                                                  */
/*   name - Is a character array of 4 bytes on a halfword boundary                  */
/*   desk - Is an integer*2 variable                                                */
/*                                                                                  */
/*==================================================================================*/

void peekt_(char * name, int      * desk)
{
  int      index;                                 /* Working index                  */
  char     taskname[TASKNAMELEN];                 /* Space for the taskname         */

  get_tname_(taskname,TASKNAMELEN);               /* Get the taskname               */
  for(index=0;index<4;index++) {                  /* Move terminal identifier       */
    name[index] = taskname[index+2L];             /* Move the byte over             */
  }                                               /*                                */
  (*desk) = (taskname[1] & 0x0F);                 /* Extract the desk number        */
  
  return;                                         /* Return to the caller           */
}                                                 /*                                */

/*==================================================================================*/
/*                                                                                  */
/* peek2()                                                                          */
/*                                                                                  */
/* taskname - Is an INTEGER*4 array of two words or a variable of 8 bytes in length.*/
/*            If TASKNAMELEN changes the length of the argument will change.        */
/*                                                                                  */
/*==================================================================================*/

void peek2_(char * taskname)
{
  get_tname_(taskname,TASKNAMELEN);               /* Get the taskname               */
  return;                                         /* Return to the caller           */
}                                                 /*                                */

/*==================================================================================*/
/*                                                                                  */
/* peek3()                                                                          */
/*                                                                                  */
/* priority - Is an INTEGER*4 variable to receive the pre-defined value.            */
/*                                                                                  */
/*==================================================================================*/

void peek3_(int      * priority)
{
  (*priority) = 120L;                             /* Priority is 120 (default)      */
  return;                                         /* Return to the caller           */
}                                                 /*                                */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
