/************************************************************************************/
/*                                                                                  */
/* ctoi.c       Convert Character String to Integer                       R00-00.00 */
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
/*   No arguments                                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        16/Apr/1999  Convert ctoi.f to ctoi.c                    */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <dlogdefs.h>

int        ctoi_(unsigned char * input, int      * chars, int      input_len)
{
  int                   index;
  int                   value;
  int                   ch;

  /*================================================================================*/
  /* Initialize the returned value and work variables                               */
  /*================================================================================*/

  value          =      0L;
  (*chars)       =      0L;

  /*================================================================================*/
  /* Skip any leading blank characters                                              */
  /*================================================================================*/

  for(index=0;index<input_len;index++) {
    if(input[index] != ' ') {
      break;
    }
  }

  /*================================================================================*/
  /* Convert the numeric bytes                                                      */
  /*================================================================================*/

  for(;index<input_len;index++) {
    ch          = (int            ) input[index];
    if(isdigit(ch)) {
      value     = (value * 10L) + (ch - '0');
      (*chars)  =  index + 1L;
    }
    else {
      break;
    }
  }

  /*================================================================================*/
  /* Return to the caller                                                           */
  /*================================================================================*/

  return(value);
}


/*==================================================================================*/
/* Main test program                                                                */
/*==================================================================================*/

#ifdef MAIN

#include <stdio.h>
#include <string.h>

static unsigned char * strings[] = {
                                    "    456  ",
                                    "a5629    ",
                                    "    32908  ",
                                    "  4579b",
                                    "345",
                                    "-0956",
                                    "   r59038     ",
                                    (unsigned char *) NULL };

int      main(int      argv, char ** argc)
{
  int         index;
  int         value;
  int         nchrs;

    dLog(DLOG_STDLOG," Value = --------, Nchrs = --------, String = <12345678901234567890>\n");

  for(index=0;strings[index]!=(unsigned char *) NULL;index++) {

    value = ctoi_(strings[index],&nchrs,strlen(strings[index]));
      dLog(DLOG_STDLOG," Value = %8d, Nchrs = %8d, String = <%s>\n",
            value,nchrs,strings[index]);
  }
  exit(0L);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
