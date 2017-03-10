/************************************************************************************/
/*                                                                                  */
/* gettname.c   Get the Assigned Taskname                                 R00-00.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains "C" definition of the routines that were located in the file  */
/* futil.f.                                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   See the individual routines                                                    */
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
/* R00-00.00  DRLJr        04/Apr/1998  File created to from futil.f                */
/* R00-00.01  DRLJr        01/Dec/1999  Check for blank as well as null character   */
/* R01-00.00 jeffsun       21/Feb/2001  Replace realtime1 with REALTIME1 for sparc  */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "private_data.h"



/*==================================================================================*/
/*                                                                                  */
/* Get_TName()                                                                      */
/*                                                                                  */
/*==================================================================================*/

void Get_TName(char * tname, int      tname_len)
{
  int         index;                             /* Working Index variable          */
  int         nobytes;                           /* Number of bytes for the name    */
  char        name[TASKNAMELEN];                 /* Working name space              */

#if 0
  slot      = private_data.my_slot - 1L;         /* Get slot number (Make 0 based)  */
  memcpy(name,REALTIME1.shared_names[slot],TASKNAMELEN);   /* Get the name          */
  if((name[0] == '\0') || (name[0] == ' ')) {    /* Empty slot ?                    */
    memset(name,(int     ) '?', TASKNAMELEN);    /* Insert question marks           */
  }                                              /*                                 */
  nobytes = ((tname_len < TASKNAMELEN) ? tname_len : TASKNAMELEN); /* Bytes to move */
  memcpy(tname,name,nobytes);                    /* Move the data                   */
  if(tname_len > TASKNAMELEN) {                  /* Do we need to fill out spaces   */
    for(index=nobytes;index<tname_len;index++) { /* Fill out the spaces             */
      tname[index] = ' ';                        /*                                 */
    }                                            /*                                 */
  }                                              /*                                 */
#else
  memcpy(name,private_data.base_task_name,TASKNAMELEN);   /* Get the name          */
  if((name[0] == '\0') || (name[0] == ' ')) {    /* Empty slot ?                    */
    memset(name,(int     ) '?', TASKNAMELEN);    /* Insert question marks           */
  }                                              /*                                 */
  nobytes = ((tname_len < TASKNAMELEN) ? tname_len : TASKNAMELEN); /* Bytes to move */
  memcpy(tname,name,nobytes);                    /* Move the data                   */
  if(tname_len > TASKNAMELEN) {                  /* Do we need to fill out spaces   */
    for(index=nobytes;index<tname_len;index++) { /* Fill out the spaces             */
      tname[index] = ' ';                        /*                                 */
    }                                            /*                                 */
  }                                              /*                                 */
#endif

  return;                                        /*                                 */
}                                                /*                                 */

void get_tname_(char * tname, int      tname_len)
{
  Get_TName(tname,tname_len);
  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
