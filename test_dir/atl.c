/****************************************************************************/
/*                                                                          */
/* atl.c       Add To Top of a circular list                      R00-00.00 */
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
/*                                                                          */
/****************************************************************************/
//  CC_Z==0 --> successful add to the list.        Not an Error.
//  CC_L==1 --> list was full.      Cannot add.    Yes Error.
//  CC_C==8 --> list was locked in cirlist_lock    Yes Error 


#include <stdio.h>
#include "list.h"

int      cirlist_lock(cirlist * list, unsigned int      type, int      param);
int      cirlist_unlock(cirlist * list, unsigned int    type);
#define  USHRT  unsigned short
#define  SLONG  int         

/*==========================================================================*/
int      atl(cirlist * list, unsigned int      value)
{
  int        cc;                           /* Return status/condition code  */
  int        slot;                         /* Slot to use                   */

#if (LIST_DEBUG_ADD)
    dLog(DLOG_DEBUG_17,"ATL of 0x%08X to List (List at 0x%08X)",
            value,list);
    LIST_PHDR("ATL",list);
#endif

  cc  = cirlist_lock(list,LIST_ATL,value); /* Lock the list                 */
  if(cc != CC_Z) {                         /* List was not locked           */
    return(cc);                            /* Return to the caller          */
  }                                        /*                               */

  if(list->nused == LIST_NSLOTS(list)) {            /* If list is full      */
    cc   =   CC_L;                                  /* List is/was full     */
  }                                                 /*                      */
  else {                                            /* Can add to the list  */
    slot = ((SLONG) list->curtop) - 1L;             /* Position to slot     */
    if(slot < 0L) {                                 /* Wrap around the list */
      slot = ((SLONG) LIST_NSLOTS(list)) - 1L;      /* The new slot to use  */
    }                                               /*                      */
    list->curtop      = (USHRT) slot;               /* Save the slot number */
    list->nused++;                                  /* Count the slot       */
    list->slots[slot] = value;                      /* Store the value      */
    cc                = CC_Z;                       /* All is okay          */
  }                                                 /*                      */
  (void) cirlist_unlock(list,LIST_ATL);             /* Unlock the list      */

#if (LIST_DEBUG_ADD)
    dLog(DLOG_DEBUG_17,"ATL of 0x%08X to List (List at 0x%08X) cc=%2d",
            value,list,cc);
    LIST_PHDR("ATL",list);
#endif

  return(cc);                                       /* Return to the caller */
}                                                   /*                      */

/*==========================================================================*/
/* atl_()    (FORTRAN Interface)                                            */
/*==========================================================================*/

void atl_(unsigned int      * value, cirlist * list, int      * status)
{
  (*status) = atl(list,(*value));
  return;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
