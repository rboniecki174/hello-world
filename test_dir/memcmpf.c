/********************************************************************/
/*                                                                  */
/* memcmpf.c      FORTRAN Interface to memcmp()           R00-00.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is an interface to the "C" library routine memory   */
/* compare function memcmp().                                       */
/*                                                                  */
/* The FORTRAN call is CALL MEMCMP(S1,S2,LEN,STA) where             */
/*                                                                  */
/*   S1  is the first string (May be any type)                      */
/*   S2  is the second string (May be any type)                     */
/*   LEN is the length of the data to compare (INTEGER*4)           */
/*   STA is the result of the compare (INTEGER*4)                   */
/*                                                                  */
/* See the memcmp() man page for status information and additional  */
/* usage information.                                               */
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

void memcmp_(char * s1, char * s2, int      * len, int      * sta)
{
  (*sta) = memcmp(s1,s2,(*len));
  return;
}

/********************************************************************/
/*                                                                  */
/********************************************************************/

