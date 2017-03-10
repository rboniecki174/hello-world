/************************************************************************************/
/*                                                                                  */
/* msleep.c     Perform a micro-second based sleep                        R00-03.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines to perform a micro-second based time sleep.      */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   ticks  - This is the address of int      (INTEGER*4) than contains the number  */
/*          - of milliseconds to sleep.                                             */
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
/* R00-00.00  DRLJr        02/Oct/1998  New set of routines created to access the   */
/*                                      fields of the circular lists.               */
/* R00-02.00  DRLJr        08/Dec/1998  Added two diagnostic error messages for bad */
/*                                      arguments.                                  */
/* R00-03.00  DRLJr        19/Apr/1999  Brought routine into standard usage based   */
/*                                      upon the new list functions.                */
// tt80093    gj           18/Jun/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <qheaders.h>
#include <dlogdefs.h>
#include "proto_common.h" //tt80093

static char errfmt[] = {
 " !!!! getlisthdr.c : Argument error - field=0x%08X, list address = 0x%08X !!!!"
};

static char errfmt1[] = {
 " !!!! getlisthdr.c :   Header Values are 0x%04hX 0x%04hX 0x%04hX 0x%04hX"
};

/*==================================================================================*/
/* FORTRAN GETLISTHDR2()                                                            */
/*==================================================================================*/

void getlisthdr2_(short      * field, cirlist * list, short      * value)
{
  int          field4;
  int          value4;

  field4    = (int     ) (*field);

  value4    = getlisthdr(field4,list);

  (*value)  = (short     ) value4;
 
  return;
}

/*==================================================================================*/
/* FORTRAN GETLISTHDR4()                                                            */
/*==================================================================================*/

void getlisthdr4_(int      * field, cirlist * list, int      * value)
{

  (*value)  = getlisthdr((*field),list);

  return;
}

/*==================================================================================*/
/* "C" getlisthdr()                                                                 */
/*==================================================================================*/

int        getlisthdr(int      field, cirlist * list)
{
  int          value;

  switch(field) {   /* Get the specific header of a circular list ................. */

    case S_AVAIL  : /* Get the number of available slots .......................... */
                    value = (int     ) (list->nslots & LOCK_MASK); /* Less lock bit */
                    break;
    case S_USED   : /* Get the number of slots that are currently in use .......... */
                    value = (int     ) (list->nused);
                    break;
    case S_TOP    : /* Get the current top slot index that is in use .............. */
                    value = (int     ) (list->curtop);
                    break;
    case S_BOT    : /* Get the next botton slot index that is to be used .......... */
                    value = (int     ) (list->nxtbot);
                    break;
    default       : /* The list header field selection is invalid ................. */
                    dLog(DLOG_MAJOR,errfmt,field,list);
                    dLog(DLOG_MAJOR,errfmt1,list->nslots,list->nused,
                                           list->curtop,list->nxtbot);
                    value   = -1L;
                    break;

  }                 /* End of the switch(field) ................................... */

  return(value);
}

/*==================================================================================*/
/* Main Test Program                                                                */
/*==================================================================================*/

#ifdef __MAIN__

int      main(int      argc, char ** argv)
{
  int          index;
  cirlist   list;
  int          value4[5];
  int          value[5];
  short         value2[5];
  int          field4[5];
  short         field2[5];

  list.nslots = 111;
  list.nused  = 222;
  list.curtop = 333;
  list.nxtbot = 444;
  
  field4[0]   = S_AVAIL4;
  field4[1]   = S_USED4;
  field4[2]   = S_TOP4;
  field4[3]   = S_BOT4;
  field4[4]   = 32;

  field2[0]   = S_AVAIL;
  field2[1]   = S_USED;
  field2[2]   = S_TOP;
  field2[3]   = S_BOT;
  field2[4]   = 32;

  for(index=0;index<5;index++) {
    getlisthdr2_(&field2[index],&list,&value2[index]);
  }
  dLog(DLOG_STDLOG,"getlisthdr2_   : %6d %6d %6d %6d %6d\n",
          value2[0],value2[1],value2[2],value2[3],value2[4]);

  for(index=0;index<5;index++) {
    getlisthdr4_(&field4[index],&list,&value4[index]);
  }
  dLog(DLOG_STDLOG,"getlisthdr4_   : %6d %6d %6d %6d %6d\n",
          value4[0],value4[1],value4[2],value4[3],value4[4]);

  for(index=0;index<5;index++) {
    value[index] = getlisthdr(index,&list);
  }
  dLog(DLOG_STDLOG,"getlisthdr     : %6d %6d %6d %6d %6d\n",
          value[0],value[1],value[2],value[3],value[4]);

  exit(0L);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
