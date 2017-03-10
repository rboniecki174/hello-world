/******************************************************************************/
/*                                                                            */
/* intasc4.c    Convert a binary number to 1-5 ASCII string         R01-01.00 */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/* Purpose:                                                                   */
/*                                                                            */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/* Arguments:                                                                 */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/* Programming Notes:                                                         */
/*                                                                            */
/* The original FORTRAN code did had the potential to overwrite memory if the */
/* value of bcnt was out of the range 1-5.  Code was added to insure that     */
/* the value of bcnt is kept in the valid range.                              */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/* Revision:                                                                  */
/*                                                                            */
/* Revision   Who          When         Why                                   */
/* =========  ===========  ===========  ===================================== */
/* R01-00.00  DRLJr        11/Nov/1999  Routine converted from FORTRAN        */
/* R01-01.00  DRLJr        09/Dec/1999  Added code to insure that potential   */
/*                                      memory overwrites do not happen.      */
/*                                                                            */
/******************************************************************************/

/*============================================================================*/
/* Include the needed system include files                                    */
/*============================================================================*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dlogdefs.h>

/*============================================================================*/
/* Processing                                                                 */
/*============================================================================*/

void intasci4_(unsigned char * asc, int      * ival, int      * bcnt)
{
  int                 index;
  int                 offset;
  int                 value;
  int                 length;
  int                 modvalue;
  int                 digit;
  unsigned char       work[6];

  if((*bcnt) > 5L) {
    dLog(DLOG_MINOR,"INTASC : WARNING: Byte Count is greater than 5 (Length is %d).",(*bcnt));
    length  = 5L;
  }
  else if((*bcnt) < 1L) {
    dLog(DLOG_MINOR,"INTASC : WARNING: Byte Count is less than 1 (Length is %d).",(*bcnt));
    length  = 1L;
  }
  else {
    length  = (*bcnt);
  }

  value     = (unsigned int     ) (*ival);
  offset    = 5L - length;
  modvalue  = 10000;
  work[6]   = '\0';

  for(index=0;index<5;index++) {
    digit       = value    / modvalue;
    work[index] = digit    + '0';
    value       = value    - (digit * modvalue);
    modvalue    = modvalue / 10;
  }
  (void)      memcpy(asc,&work[offset],length);
  return;
}

/*============================================================================*/
/* Main Test Program                                                          */
/*============================================================================*/

#ifdef MAIN

int      main(int             argc, char ** argv)
{
  int             index;
  int             value;
  int             bcnt; 
  int             length;
  int             status;
  unsigned char    asc[6];

  for(index=1;index<argc;index+=2) {

    memset(asc,'\0',6L);

    status = 0L;
    bcnt   = atoi(argv[index+0]);    /* Length of the byte field  */
    value  = atoi(argv[index+1]);
    length = strlen(argv[index+1]);

    (void)   intasc_(asc,&value,&bcnt);

    dLog(DLOG_STDLOG,"Input=<%-20s>, BCNT=%3d, Value=%6d <%s>",
            argv[index+1],bcnt,value,asc);

  }

  exit(0L);
}

#endif

/******************************************************************************/
/*                                                                            */
/******************************************************************************/

