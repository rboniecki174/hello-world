/********************************************************************/
/*                                                                  */
/* memsetf.c      FORTRAN Interface to memset()           R00-00.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is an interface to the "C" library routine memory   */
/* compare function memset().                                       */
/*                                                                  */
/* The FORTRAN call is CALL MEMSET(S1,VALUE,LEN) where              */
/*                                                                  */
/*   S1    is the memory set be set from value (May be any type)    */
/*   VALUE is the value to use to set the address at S1 (INTEGER*4) */
/*   LEN   is the length of the data to set (INTEGER*4)             */
/*                                                                  */
/* See the memset() man page for move information.                  */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  13/Apr/1997  DRLJr     New Function                   */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <string.h>

void memset_(char * s1, int      * value, int      * len)
{
  (void) memset(s1,(*value),(*len));
  return;
}

/********************************************************************/
/*                                                                  */
/********************************************************************/

