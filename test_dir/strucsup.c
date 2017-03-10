/*********************************************************************************/
/*                                                                               */
/* strucsup.c      Structure Storage Support functions (FORTRAN)       R00-00.00 */
/*                                                                               */
/* The functions in this source module are intended to allow for the storing of  */
/* fullwords, 3 byte 'fullwords', halfwords and bytes into a 'psuedo' structure  */
/* variable that is defined in FORTRAN.  The data is store in the buffer in the  */
/* Big Endian or Network byte order (1st byte is the most signficant).           */
/*                                                                               */
/* Each of the functions is a FORTRAN subroutine and takes three arguments:      */
/*                                                                               */
/*    valuep  = This is a pointer to an INTEGER*4 variable/array element.        */
/*    buffer  = This is a pointer to the buffer being used as a structure.       */
/*    offset  = This is a pointer to an INTEGER*4 variable/array element that    */
/*              contains the offset into the 'structure' where the data is to be */
/*              stored.  This a 1 based offset and not a 0 (zero) based offset.  */
/*                                                                               */
/* The subroutines/function names ("C" names are in ()) are:                     */
/*                                                                               */
/*   STBYTE  (stbyte_)     Store a byte of data                                  */
/*   STHALF  (sthalf_)     Store a halfword of data                              */
/*   STFULL  (stfull_)     Store a fullword of data                              */
/*   STFULL3 (stfull3_)    Store the least significant 3 bytes of a fullword     */
/*                                                                               */
/*   LDBYTE  (ldbyte_)     Get a byte of data                                    */
/*   LDHALF  (ldhalf_)     Get a halfword of data                                */
/*   LDFULL  (ldfull_)     Get a fullword of data                                */
/*   LDFULL3 (ldfull3_)    Get the least significant 3 bytes of a fullword       */
/*                                                                               */
/* All of the routines are called as FORTRAN Subroutines.  STFULL3/LDFULL3       */
/* store and receive 3 bytes from the buffer.  The arguments for the value and   */
/* offset are FORTRAN INTEGER*4 variables.  The buffer argument may be any       */
/* legal variable type.  If CHARACTER variable have a hidden length that is      */
/* placed between the buffer address value and the address of the offset (OS/32  */
/* FORTRAN Compiler) then CHARACTER variables may not be used for the buffer.    */
/*                                                                               */
/* FORTRAN Examples:                                                             */
/*                                                                               */
/*    INTEGER*4    VALUE,BUFFER(10),OFFSET                                       */
/*                                                                               */
/*    VALUE  = 1                                                                 */
/*    OFFSET = 5 (2nd Word start location)                                       */
/*    CALL STBYTE(VALUE,BUFFER,OFFSET)                                           */
/*                                                                               */
/*    CALL LDFULL(VALUE,BUFFER,OFFSET)                                           */
/*                                                                               */
/* To compile the code with the built in test program execute the following (or  */
/* appropriate) command:                                                         */
/*                                                                               */
/*    cc -o strucsup -DMAIN strucsup.c                                           */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

#ifdef MAIN
#include <stdio.h>
#include <string.h>
#endif

#define UCHR  unsigned char
#define ULI   unsigned int     

#define N256  (         1)
#define S256  (       256)
#define D256  (S256 * 256)
#define T256  (D256 * 256)
#define M256  (S256 -   1)

void stbyte_(ULI      * valuep,         /* Pointer to the value to store         */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */

  offs    =  (*offset);                 /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */
  value   =  (*valuep);                 /* Get the value as a fullword           */

  buffer[offs] = (UCHR) (value & M256); /* Store the byte into the 'structure'   */

  return;                               /*                                       */
}                                       /*                                       */


void sthalf_(ULI      * valuep,         /* Pointer to the value to store         */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[2];                   /* Bytes to hold the data                */

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */
  value      =  (*valuep);              /* Get the value as a fullword           */

  byte[0]    =  (UCHR) ((value / S256) & M256);
  byte[1]    =  (UCHR) ((value / N256) & M256);

  buffer[offs+0] = byte[0];             /* Store the first byte                  */
  buffer[offs+1] = byte[1];             /* Store the second byte                 */

  return;                               /*                                       */
}                                       /*                                       */


void stfull_(ULI      * valuep,         /* Pointer to the value to store         */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[4];

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */
  value      =  (*valuep);              /* Get the value as a fullword           */

  byte[0]    =  (UCHR) ((value / T256) & M256);
  byte[1]    =  (UCHR) ((value / D256) & M256);
  byte[2]    =  (UCHR) ((value / S256) & M256);
  byte[3]    =  (UCHR) ((value / N256) & M256);

  buffer[offs+0] = byte[0];       /* Store the first byte                  */
  buffer[offs+1] = byte[1];       /* Store the second byte                 */
  buffer[offs+2] = byte[2];       /* Store the third byte                  */
  buffer[offs+3] = byte[3];       /* Store the fourth byte                 */

  return;                               /*                                       */
}                                       /*                                       */

void stfull3_(ULI      * valuep,        /* Pointer to the value to store         */
              UCHR     * buffer,        /* Address of 'structure' for value      */
              ULI      * offset)        /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[4];

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */
  value      =  (*valuep);              /* Get the value as a fullword           */

  byte[0]    =  (UCHR) ((value / T256) & M256);
  byte[1]    =  (UCHR) ((value / D256) & M256);
  byte[2]    =  (UCHR) ((value / S256) & M256);
  byte[3]    =  (UCHR) ((value / N256) & M256);

  buffer[offs+0] = byte[1];       /* Store the first byte                  */
  buffer[offs+1] = byte[2];       /* Store the second byte                 */
  buffer[offs+2] = byte[3];       /* Store the third byte                  */

  return;                               /*                                       */
}                                       /*                                       */

void ldbyte_(ULI      * valuep,         /* Pointer to the value to load          */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */

  offs    =  (*offset);                 /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */

  value   =  buffer[offs];              /* Get the byte from the 'structure'     */
  value  &=  M256;                      /* Insure not sign extended              */

  (*valuep) = value;                    /* Return to the caller                  */

  return;                               /*                                       */
}                                       /*                                       */


void ldhalf_(ULI      * valuep,         /* Pointer to the value to store         */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[2];

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */

  byte[0]    =  buffer[offs+0];
  byte[1]    =  buffer[offs+1];

  value      =  (byte[0] * S256) +
                (byte[1] * N256) ;

  (*valuep)     = value;                /* Return value to caller                */

  return;                               /*                                       */
}                                       /*                                       */


void ldfull_(ULI      * valuep,         /* Pointer to the value to store         */
             UCHR     * buffer,         /* Address of 'structure' for value      */
             ULI      * offset)         /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[4];

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */

  byte[0]    =  buffer[offs+0];
  byte[1]    =  buffer[offs+1];
  byte[2]    =  buffer[offs+2];
  byte[3]    =  buffer[offs+3];

  value      =  (byte[0] * T256) +
                (byte[1] * D256) +
                (byte[2] * S256) +
                (byte[3] * N256) ;

  (*valuep)     = value;                /* Return value to caller                */

  return;                               /*                                       */
}                                       /*                                       */

void ldfull3_(ULI      * valuep,        /* Pointer to the value to store         */
              UCHR     * buffer,        /* Address of 'structure' for value      */
              ULI      * offset)        /* Offset (1 based) into the 'structure' */
{
  ULI        offs;                      /* Zero based offset into the buffer     */
  ULI        value;                     /* Actual value to be store              */
  UCHR       byte[4];                   /*                                       */

  offs       =  (*offset);              /* Get the offset in the structure       */
  offs--;                               /* Convert to zero based offset          */

  byte[0]    =        (UCHR) 0;
  byte[1]    =  buffer[offs+0];
  byte[2]    =  buffer[offs+1];
  byte[3]    =  buffer[offs+2];

  value      =  (byte[0] * T256) +
                (byte[1] * D256) +
                (byte[2] * S256) +
                (byte[3] * N256) ;


  (*valuep)     = value;                /* Return value to caller                */

  return;                               /*                                       */
}                                       /*                                       */

/*===============================================================================*/
/* Test Program                                                                  */
/*                                                                               */
/* Note: Test program operates on assumption of ASCII UCHRacter codes.           */
/*                                                                               */
/*===============================================================================*/

#ifdef MAIN 

ULI      main(ULI      argc, UCHR ** argv)
{
  ULI         index;
  ULI         offset;
  UCHR        buffer[64];
  ULI         value;
  ULI         result;
  ULI         answers[10];

  /*-----------------------------------------------------------------------------*/
  /* Byte Storage Test                                                           */
  /*-----------------------------------------------------------------------------*/

  (void) memset(buffer,(ULI     ) ' ',64L);
  for(index=0;index<10;index++) {
    value  = index + 0x30;
    answers[index] = value;
    offset = index + 1;
    stbyte_(&value,buffer,&offset);
  }
  fprintf(stdout,"stbyte_()  = <%-64.64s>\n",buffer);

  for(index=0;index<10;index++) {
    offset = index + 1;
    ldbyte_(&result,buffer,&offset);
    fprintf(stdout,"ldbyte_()  = <%1c> out=%08X, in=%08X \n",
            (UCHR) result,result, answers[index]);
  }

  fprintf(stdout,"\n");

  /*-----------------------------------------------------------------------------*/
  /* Halfword Storage Test                                                       */
  /*-----------------------------------------------------------------------------*/

  (void) memset(buffer,(ULI     ) ' ',64L);
  for(index=0;index<10;index++) {
    value  = (index + 0x30) + ((index + 0x40) * S256);
    answers[index] = value;
    offset = (index * 2) + 1;
    sthalf_(&value,buffer,&offset);
  }
  fprintf(stdout,"sthalf_()  = <%-64.64s>\n",buffer);

  for(index=0;index<10;index++) {
    offset = (index * 2) + 1;
    ldhalf_(&result,buffer,&offset);
    fprintf(stdout,"ldhalf_()  = <%1c> <%1c>  out=%08X, in=%08X  \n",
            (UCHR) (result / S256),
            (UCHR) (result / N256),
            result,answers[index]);
  }

  fprintf(stdout,"\n");

  /*-----------------------------------------------------------------------------*/
  /* Fullword Storage Test                                                       */
  /*-----------------------------------------------------------------------------*/

  (void) memset(buffer,(ULI     ) ' ',64L);
  for(index=0;index<10;index++) {
    value  = ((index + 0x30) *    1) + ((index + 0x40) *  256) +
             ((index + 0x50) * D256) + ((index + 0x70) * T256) ;
    answers[index] = value;
    offset = (index * 4) + 1;
    stfull_(&value,buffer,&offset);
  }
  fprintf(stdout,"stfull_()  = <%-64.64s>\n",buffer);

  for(index=0;index<10;index++) {
    offset = (index * 4) + 1;
    ldfull_(&result,buffer,&offset);
    fprintf(stdout,"ldfull_()  = <%1c> <%1c> <%1c> <%1c> out=%08X, in=%08X\n",
            (UCHR) (result / T256),(UCHR) (result / S256), 
            (UCHR) (result / S256),(UCHR) (result / N256),
            result,answers[index]);
  }

  fprintf(stdout,"\n");

  /*-----------------------------------------------------------------------------*/
  /* Fullword (3 Bytes) Storage Test                                             */
  /*-----------------------------------------------------------------------------*/

  (void) memset(buffer,(ULI     ) ' ',64L);
  offset   = 1;
  for(index=0;index<10;index++) {
    value  = ((index + 0x30) *    1) + ((index + 0x40) *  256) +
             ((index + 0x50) * D256) + ((index + 0x70) * T256) ;
    answers[index] = value;
    stfull3_(&value,buffer,&offset);
    offset += 3;
  }
  fprintf(stdout,"stfull3_() = <%-64.64s>\n",buffer);

  offset   = 1;
  for(index=0;index<10;index++) {
    ldfull3_(&result,buffer,&offset);
    offset += 3;
    fprintf(stdout,"ldfull3_() = <%1c> <%1c> <%1c> <%1c> out=%08X, in=%08X\n",
            (UCHR) (result / T256),(UCHR) (result / S256), 
            (UCHR) (result / S256),(UCHR) (result / N256),
            result,answers[index]);
  }

  fprintf(stdout,"\n");

  /*-----------------------------------------------------------------------------*/
  /*                                                                             */
  /*-----------------------------------------------------------------------------*/

  exit(0);
}

#endif

/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/

