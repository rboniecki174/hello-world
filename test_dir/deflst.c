/****************************************************************************/
/*                                                                          */
/* deflst.c    Create a standard circular list                    R00-00.00 */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Programing Notes:                                                        */
/*                                                                          */
/* This routine is a replacement for the list.f subroutine.                 */      
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Revision History:                                                        */
/*                                                                          */
/* Revision   Date         Whom      Why                                    */
/* =========  ===========  ========  ====================================== */
/* R00-00.00  15/Apr/1999  DRLJr     Replacement for list.f                 */
/* R00-02.00  20/Feb/2001  PT        Mods for Sparc posix locks             */
// tt80093    29/May/2014  gj        Clean up prototypes
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include "list.h"
#include <dlogdefs.h>
#include "proto_common.h" //tt80093

/*==========================================================================*/
/*                                                                          */
/*==========================================================================*/

//int posix_lock_init (posix_mlock_t *lock); //tt80093

static const char errmsg[] =                      /* PT - 02/20/01 */
  { " deflst  : Circular list size error.  Size of %d forced to 0.\n" };

/*==========================================================================*/
/* deflst()  ("C" Interface)                                                */
/*==========================================================================*/

void deflst(cirlist * list, int      size)
{
#ifdef posix     /* PT - 02/20/01 */
  int status;

  /* initialize the posix lock */
  if ((status = posix_lock_init (&list->lock)) != 0x00) {
    dLog(DLOG_MAJOR,"deflst: Lock initialization error");
    size       = 0L;
  }
#endif
  if((size < 1L) || (size > 32767)) {
    dLog(DLOG_MAJOR,errmsg, size);
    size       = 0L;
  }

  list->nslots = (unsigned short     ) size;
  list->nused  = (unsigned short     ) 0L;
  list->curtop = (unsigned short     ) 0L;
  list->nxtbot = (unsigned short     ) 0L;
  return;
}

/*==========================================================================*/
/* deflst_()  (FORTRAN Interface)                                           */
/*==========================================================================*/

void deflst_(cirlist * list, int      * size)
{
  deflst(list,(*size));
  return;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
