/************************************************************************************/
/*                                                                                  */
/* filwrd.c       Fill a Halfword Array with the specified character      R01-00.00 */
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
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* This is the original FORTRAN code                                                */
/*                                                                                  */
/*    SUBROUTINE FILWRD (ARAY, CHAR, FROM, FOR)                                     */
/*    INTEGER*2 FROM, FOR                          ! Byte indices in record         */
/*    LOGICAL*1 ARAY( * ), CHAR( 2 )                                                */
/*    INTEGER*2 TO, I                                                               */
/*                                                                                  */
/*    TO = (FROM+FOR) - 1                                                           */
/*    DO I = FROM, TO                                                               */
/*       ARAY( I ) = CHAR( 2 )               <<< Note: Use 2nd byte to fill !!!!    */
/*    ENDDO                                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        05/Jul/2000  Replaced FORTRAN Function                   */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Needed Include Files                                                             */
/*==================================================================================*/

#include <string.h>

/*==================================================================================*/
/* FORTRAN Interface                                                                */
/*==================================================================================*/

void filwrd_(unsigned char      * aray, unsigned char      * chr,
             short              * from, short              * forr)
{
  int                 length;                            /* Length to fill          */
  int                 offset;                            /* Offset to start at      */
  unsigned int        byte;                              /* Byte to fill with       */

  offset = (int            ) (*from) - 1L;               /* Start Location          */
  length = (int            ) (*forr);                    /* Length to change        */
  byte   = (unsigned int     ) chr[1];                   /* Character to use        */
  (void)   memset(&aray[offset],byte,length);            /* Do the fill             */
  return;                                                /*                         */
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
