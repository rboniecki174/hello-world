/********************************************************************/
/*                                                                  */
/* memcpyf.c      FORTRAN Interface to memcpy()           R00-00.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is an interface to the "C" library routine memory   */
/* compare function memcpy().                                       */
/*                                                                  */
/* The FORTRAN call is CALL MEMCPY(S1,S2,LEN) where                 */
/*                                                                  */
/*   S1  is the first string (May be any type)                      */
/*   S2  is the second string (May be any type)                     */
/*   LEN is the length of the data to copy (INTEGER*4)              */
/*                                                                  */
/* See the memcpy() man page for more information.                  */
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

void memcpy_(char * s1, char * s2, int      * len)
{
  (void) memcpy(s1,s2,(*len));
  return;
}

/********************************************************************/
/*                                                                  */
/********************************************************************/

