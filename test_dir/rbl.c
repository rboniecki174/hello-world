/****************************************************************************/
/*                                                                          */
/* rbl.c       Remove from Bottom of a Circular List              R00-00.00 */
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

#include <stdio.h>
#include "list.h"

#define  USHRT  unsigned short
#define  SLONG  int         

/*==========================================================================*/
/* rbl()     ("C" Interface)                                                */
/*==========================================================================*/
/*  CC_G==2 --> list is empty and returns an error on the get.  ERROR!
 *  CC_Z==0 --> list is empty after a successful get.          Not an Error.
 *  CC_L==1 --> list still has stuff after a successful get.   Not an Error.
 */
int      cirlist_lock(cirlist * list, unsigned int      type, int      param);
int      cirlist_unlock(cirlist * list, unsigned int    type);

int      rbl(cirlist * list, unsigned int      * value)
{
  int        cc;                           /* Return status/condition code  */
  int        slot;                         /* Slot to use                   */

#if (LIST_DEBUG_REM)
    dLog(DLOG_DEBUG_17,"RBL of %8.8s to List (List at 0x%08X)",
            "fullword",list);
     LIST_PHDR("RBL",list);
#endif

  cc  = cirlist_lock(list,LIST_RBL,*value); /* Lock the list                 */
  if(cc != CC_Z) {                         /* List was not locked           */
    return(cc);                            /* Return to the caller          */
  }                                        /*                               */

  if(list->nused == (USHRT) 0L) {                   /* If list is empty     */
    cc   =   CC_G;                                  /* List is empty        */
  }                                                 /*                      */
  else {                                            /* Can add to the list  */
    slot = ((SLONG) list->nxtbot) - 1L;             /* Position to slot     */
    if(slot < 0L) {                                 /* List has wrapped     */
      slot = LIST_NSLOTS(list) - 1L;                /* The next bottom      */
    }                                               /*                      */
    (*value)     = list->slots[slot];               /* Return value         */
#ifdef MAIN
    list->slots[slot] = 0xFFFFFFFF;
#endif
    list->nxtbot = slot;                            /* Position to slot     */
    list->nused--;                                  /* Count the slot       */
    cc = ((list->nused == 0L) ? CC_Z : CC_L);       /* Set the cc/status    */
  }                                                 /*                      */

#if (LIST_DEBUG_REM)
    dLog(DLOG_DEBUG_17,"RBL of 0x%08X from List (List at 0x%08X) cc=%2d",
            (*value),list,cc);
     LIST_PHDR("RBL",list);
#endif

  (void) cirlist_unlock(list,LIST_RBL);             /* Unlock the list      */
  return(cc);                                       /* Return to the caller */
}                                                   /*                      */

/*==========================================================================*/
/* rbl_()    (FORTRAN Interface)                                            */
/*==========================================================================*/
void rbl_(unsigned int      * value, cirlist * list, int      * status)
{
  (*status) = rbl(list,value);
  return;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
