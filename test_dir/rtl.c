/****************************************************************************/
/*                                                                          */
/* rtl.c       Remove from Top of a circular list                 R00-00.00 */
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
// tt80093    22/May/2014  gj        Clean up prototypes
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include "list.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define  USHRT  unsigned short
#define  SLONG  int         

/*==========================================================================*/
/* rtl()     ("C" Interface)                                                */
/*==========================================================================*/

int      rtl(cirlist * list, unsigned int      * value)
{
  int        cc;                           /* Return status/condition code  */
  int        slot;                         /* Slot to use                   */

#if (LIST_DEBUG_REM)
    fprintf(stderr,"RTL of %8.8s to List (List at 0x%08X)\n",
            "fullword",list);
     LIST_PHDR("RTL",list);
#endif

  cc  = cirlist_lock(list,LIST_RTL,*value); /* Lock the list                 */
  if(cc != CC_Z) {                         /* List was not locked           */
    return(cc);                            /* Return to the caller          */
  }                                        /*                               */

  if(list->nused == (USHRT) 0L) {                   /* If list is empty     */
    cc   =   CC_G;                                  /* List is empty        */
  }                                                 /*                      */
  else {                                            /* Can add to the list  */
    (*value) = list->slots[list->curtop];           /* Return value         */
#ifdef MAIN
    list->slots[list->curtop] = 0xFFFFFFFF;
#endif
    slot = ((SLONG) list->curtop) + 1L;             /* Position to slot     */
    if(slot >= (SLONG) LIST_NSLOTS(list)) {         /* Wrap around the list */
      list->curtop    = (USHRT) 0L;                 /* Save the slot number */
    }                                               /*                      */
    else {                                          /* Save new value       */
      list->curtop    = (USHRT) slot;               /*                      */
    }                                               /*                      */
    list->nused--;                                  /* Count the slot       */
    cc = ((list->nused == 0L) ? CC_Z : CC_L);       /* Set the cc/status    */
  }                                                 /*                      */

#if (LIST_DEBUG_REM)
    fprintf(stderr,"RTL of 0x%08X to List (List at 0x%08X) cc=%2d\n",
            (*value),list,cc);
     LIST_PHDR("RTL",list);
#endif

  (void) cirlist_unlock(list,LIST_RTL);       /* Unlock the list      */
  return(cc);                                       /* Return to the caller */
}                                                   /*                      */

/*==========================================================================*/
/* rtl_()    (FORTRAN Interface)                                            */
/*==========================================================================*/

void rtl_(unsigned int      * value, cirlist * list, int      * status)
{
  (*status) = rtl(list,value);
  return;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
