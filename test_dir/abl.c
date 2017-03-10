/****************************************************************************/
/*                                                                          */
/* abl.c       Add To Bottom of a circular list                   R00-00.00 */
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
// tt80093    18/Jun/2014  gj        Clean up prototypes
/****************************************************************************/

#include <stdio.h>
#include "list.h"
#include "proto_common.h" //tt80093

#define  USHRT  unsigned short
#define  SLONG  int         
//int cirlist_lock(cirlist * list, unsigned int type, int param); //tt80093
//int cirlist_unlock(cirlist * list, unsigned int type);          //tt80093

/*==========================================================================*/
/* abl()     ("C" Interface)                                                */
/*==========================================================================*/

int      abl(cirlist * list, unsigned int      value)
{
  int        cc = 0;                       /* Return status/condition code  */
  int        slot;                         /* Slot to use                   */

#if (LIST_DEBUG_ADD)
    fprintf(stderr,"ABL of 0x%08X to List (List at 0x%08X) cc=%2d\n",
            value,(unsigned int)list,cc);
    LIST_PHDR("ABL",list);
#endif

  cc  = cirlist_lock(list,LIST_ABL,value); /* Lock the list                 */
  if(cc != CC_Z) {                         /* List was not locked           */
    return(cc);                            /* Return to the caller          */
  }                                        /*                               */

  if(list->nused == LIST_NSLOTS(list)) {            /* If list is full      */
    cc   =   CC_L;                                  /* List is/was full     */
  }                                                 /*                      */
  else {                                            /* Can add to the list  */
    slot                 = list->nxtbot;            /* Get the slot number  */
    list->nused++;                                  /* Count the slot       */
    list->nxtbot++;                                 /* Move to next slot    */
    if(list->nxtbot >= LIST_NSLOTS(list)) {         /* Check for list wrap  */
      list->nxtbot       =  0L;                     /* Set to bottom slot   */
    }                                               /*                      */
    list->slots[slot]    = value;                   /* Store the value      */
    cc                   = CC_Z;                    /* All is okay          */
  }                                                 /*                      */

#if (LIST_DEBUG_ADD)
    fprintf(stderr,"ABL of 0x%08X to List (List at 0x%08X)\n",
            value,(unsigned int)list);
    LIST_PHDR("ABL",list);
#endif

  (void) cirlist_unlock(list,LIST_ABL);             /* Unlock the list      */
  return(cc);                                       /* Return to the caller */
}                                                   /*                      */

/*==========================================================================*/
/* abl_()    (FORTRAN Interface)                                            */
/*==========kkkk================================================================*/

void abl_(unsigned int      * value, cirlist * list, int      * status) 
{
  (*status) = abl(list,(*value));
  return; 
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
