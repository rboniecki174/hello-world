/********************************************************************/
/*                                                                  */
/* ato.c      FORTRAN Interface to atoi and atof         R00-00.00  */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/*  ATOI and ATOF for use in fortran code                           */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  07/May/2009  Slava     New Function                   */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


int atoi_(char * s1)
{
  return atoi((const char *)s1);
}

double atof_(char * s1)
{
  return atof((const char *)s1);
}

 
/********************************************************************/
/*                                                                  */
/********************************************************************/

