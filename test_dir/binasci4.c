/******************************************************************************/
/*                                                                            */
/* binasci4.c   Convert a binary number to 2 digit ASCII string     R01-00.01 */
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
/* The FORTRAN routine did not return any string if the input value was from  */
/* 100 to 999.                                                                */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/* Revision:                                                                  */
/*                                                                            */
/* Revision   Who          When         Why                                   */
/* =========  ===========  ===========  ===================================== */
/* R01-00.00  DRLJr        11/Nov/1999  Routine converted from FORTRAN        */
/* R01-00.01  DRLJr        09/Dec/1999  Minor changes to the test program     */
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

void binasci4_(int             * binrin, unsigned char * asc)
{
  int                   work;
  unsigned char         huns;
  unsigned char         tens;
  unsigned char         ones;

  if(((*binrin) >= 0) && ((*binrin) <= 99)) {
    tens   =  ((*binrin) / 10) + '0';
    ones   =  ((*binrin) % 10) + '0';
    asc[0] =  tens;
    asc[1] =  ones;
  }
  else if(((*binrin) >= 100) && ((*binrin) <= 999)) {
    huns   =  ((*binrin) / 100) + '0';
    work   =  (*binrin)  - (((*binrin) / 100) * 100);
    tens   =  (work      / 10) + '0';
    ones   =  (work      % 10) + '0';
    asc[0] =  huns;
    asc[1] =  tens;
    asc[2] =  ones;
  }
  else {
    dLog(DLOG_MINOR,"BINASC : Range Error on input value (%d,0x%04hX).  Range is 0 - 999.",
            (*binrin),(*binrin));
  }

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
  int             length;
  int             status;
  unsigned char    asc[4];

  for(index=1;index<argc;index++) {

    memset(asc,'\0',4L);

    status = 0L;
    length = strlen(argv[index]);
    value  = atoi(argv[index]);
    (void)   binasc_(&value,asc);

    dLog(DLOG_STDLOG,"Input=<%-20s>, Status=%6d, Value=%6d, Output=<%3s>",
            argv[index],status,value,asc);

  }

  exit(0L);
}

#endif


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
