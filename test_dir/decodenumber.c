/************************************************************************************/
/*                                                                                  */
/* DecodeNumber()  Extract and Convert a Numeric value from the buffer    R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the function that will convert a decimal or hexadecimal value */
/* in the passed buffer to binary.  The formats supported are:                      */
/*                                                                                  */
/*           [-]<decimal-digits>                                                    */
/*           [+]<decimal-digits>                                                    */
/*           x<hexadecimal-digits>                                                  */
/*           0x<hexadecimal-digits>                                                 */
/*                                                                                  */
/* decimal-digits     is a string of bytes in the range '0' to '9'                  */
/* hexadecimal-digits is a string of bytes in the range '0' to '9', 'A' - 'F' or    */
/*                    'a' - 'f'                                                     */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   Various based upon the arguments of the functions.                             */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        29/Mar/1999  New file created for process_asn support    */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include Files                                                                    */
/*==================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/*==============================================================================*/
/* DecodeNumber() : Extract and Convert a Numeric value from the buffer         */
/*==============================================================================*/

int      DecodeNumber(unsigned char * buffer, int      * rtnvalue)
{
  int              status;                            /* Working status         */
  int              rtnstatus;                         /* Return status          */
  int              byte;                              /* Working byte           */
  int              value;                             /* Value extracted        */
  int              offset;                            /* Offset into string     */
  int              sign;                              /* Sign of the number     */
  int              base;                              /* Base of number (10/16) */
  int              count;                             /* Number of digits       */

  /*----------------------------------------------------------------------------*/
  /* Extract the number                                                         */
  /*----------------------------------------------------------------------------*/

  count       =    0L;                                /* Number digits converted*/
  offset      =    0L;                                /* Start at beginning     */
  value       =    0L;                                /* Set value to 0         */
  sign        =   +1L;                                /* Assume positive        */
  base        =   10L;                                /* Select Decimal Mode    */

  if(buffer[offset] == '-') {                         /* Negative number        */
    sign      =   -1L;                                /* Save sign state        */
    offset++;                                         /* Move over sign         */
  }                                                   /*                        */
  else if(buffer[offset] == '+') {                    /* Postive number         */
    sign      =   +1L;                                /* Save sign state        */
    offset++;                                         /* Move over sign         */
  }                                                   /*                        */
  if((buffer[offset+1L]=='x') || (buffer[offset+1L]=='x')) {
    sign      =   +1L;                                /* Always Positive        */
    base      =  +16L;                                /* Convert the base       */
    offset++;                                         /* Move over base         */
  }                                                   /*                        */
  else if(buffer[offset+0L] == '0') {                 /* Check for possible hex */
    if((buffer[offset+1L]=='x') || (buffer[offset+1L]=='x')) {
      sign      =   +1L;                              /* Always Positive        */
      base      =  +16L;                              /* Convert the base       */
      offset   +=    2L;                              /* Move over base         */
    }                                                 /*                        */
  }                                                   /*                        */

  rtnstatus = 0L;                                     /* Indicate good status   */
  for(;buffer[offset]!='\0';offset++) {               /* Extract the value      */
    byte   = buffer[offset];                          /* Extract the byte       */
    status = isdigit((int     ) byte);                /* Numeric (0-9)          */
    if((! status) && (base == 16L)) {                 /* Check for A-Z ?        */
      status = isxdigit((int     ) byte);             /* Hexadecimal (A-F) ?    */
      if(status) {                                    /* A-F or a-f             */
        count++;                                      /* Count the digit        */
        byte  = toupper((int     ) byte);             /* Insure its uppercase   */
        byte  = byte - 'A';                           /* Make 0 to 5            */
        byte  = byte + '0' + 10L;                     /* Cnv to char above '9'  */
      }                                               /*                        */
    }                                                 /*                        */
    if(status) {                                      /* Yes, convert the byte  */
      count++;                                        /* Count the digit        */
      value           = (value * base) + (byte - '0');/* Convert the number     */
    }                                                 /*                        */
    else {                                            /* No, exit loop          */
      rtnstatus       = -1L;                          /* Bad Character          */
      break;                                          /* Keyword extracted      */
    }                                                 /*                        */
  }                                                   /*                        */

  if(rtnstatus == 0L) {                               /* Return the value found */
    if(count != 0L) {                                 /* Converted at least one */
      (*rtnvalue) = value * sign;                     /* Return the value       */
    }                                                 /*                        */
    else {                                            /* No digits converted    */
      rtnstatus       = -2L;                          /* Bad Character          */
    }                                                 /*                        */
  }                                                   /*                        */

  return(rtnstatus);                                  /* Return to the caller   */

} /* End of function .......................................................... */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
