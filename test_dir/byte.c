/********************************************************************/
/*                                                                  */
/* byte.c   FORTRAN Callable Byte Manipulation Routines   R00-01.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is a replacement for the byte.f subroutine.         */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  14/Apr/1999  DRLJr     Replacement for byte.f         */
/* R00-01.00  17/May/1999  DRLJr     The original FORTRAN would sign*/
/*                                   byte that was returned to the  */
/*                                   caller due to the nature of the*/
/*                                   definition of the FORTRAN type */
/*                                   BYTE.  Added code in ilbyte to */
/*                                   simulate that behavior until   */
/*                                   the repercusion can be resolved*/
/*                                                                  */
/********************************************************************/

#define UCHAR unsigned char

#define SIGN_EXTEND       1     /* 1 = Match byte.f sign extend bug */
                                /* 0 = Use proper unsigned operation*/

/*==================================================================*/
/* ilbyte_() (FORTRAN Interface)                                    */
/*==================================================================*/

void ilbyte_(int      * value, UCHAR * bytes, int      * offset) 
{
  (*value) = bytes[(*offset)];           /* Get the byte to return  */
#if SIGN_EXTEND
  if(((*value) & 0x80) == 0x80) {        /* Sign Extend the value   */
    (*value) |= 0xFFFFFF00;              /* Do original byte.f bug  */
  }
#endif
  return;                                /* Return to the caller    */
}                                        /*                         */

/*==================================================================*/
/* isbyte_() (FORTRAN Interface)                                    */
/*==================================================================*/

void isbyte_(int      * value, UCHAR * bytes, int      * offset) 
{
  bytes[(*offset)] = (UCHAR) (*value);   /* Store the byte          */
  return;                                /* Return to the caller    */
}                                        /*                         */

/*==================================================================*/
/* icbyte_() (FORTRAN Interface)                                    */
/*==================================================================*/

void icbyte_(UCHAR * bytes, int      * offset) 
{
  bytes[(*offset)] = (UCHAR) 0L;         /* Clear specified byte    */
  return;                                /* Return to the caller    */
}                                        /*                         */

/*==================================================================*/
/* icbyte_() (FORTRAN Interface)                                    */
/*==================================================================*/

void inbyte_(UCHAR * bytes, int      * offset) 
{
  bytes[(*offset)] = ~(bytes[(*offset)]);/* Invert specified byte   */
  return;                                /* Return to the caller    */
}                                        /*                         */


/*==================================================================*/
/* Test Program                                                     */
/*==================================================================*/

#ifdef MAIN

#include <stdio.h>
#include <dlogdefs.h>


int      main(int      argc, char ** argv)
{
  int         index;
  int         value;
  UCHAR       array[24];

  dLog(DLOG_MINOR,"Initial Setting ");
  value    = 0xFF;
  for(index=0;index<24;index++) {
    isbyte_(&value,array,&index);
    dLog(DLOG_MINOR," %02.2X",array[index]);
  }
  dLog(DLOG_MINOR,"\n");

  dLog(DLOG_MINOR,"Store 0,1,2...  ");
  for(index=0;index<24;index++) {
    isbyte_(&index,array,&index);
    dLog(DLOG_MINOR," %02.2X",array[index]);
  }
  dLog(DLOG_MINOR,"\n");

  dLog(DLOG_MINOR,"Get 0,1,2...    ");
  for(index=0;index<24;index++) {
    ilbyte_(&value,array,&index);
    dLog(DLOG_MINOR," %02.2X",value);
  }
  dLog(DLOG_MINOR,"\n");

  dLog(DLOG_MINOR,"Second Setting  ");
  value    = 0xFF;
  for(index=0;index<24;index++) {
    isbyte_(&value,array,&index);
    dLog(DLOG_MINOR," %02.2X",array[index]);
  }
  dLog(DLOG_MINOR,"\n");

  dLog(DLOG_MINOR,"Clearing Bytes  ");
  for(index=0;index<24;index++) {
    icbyte_(array,&index);
    dLog(DLOG_MINOR," %02.2X",array[index]);
  }
  dLog(DLOG_MINOR,"\n");

  dLog(DLOG_MINOR,"Inverting Bytes ");
  for(index=0;index<24;index++) {
    inbyte_(array,&index);
    dLog(DLOG_MINOR," %02.2X",array[index]);
  }
  dLog(DLOG_MINOR,"\n");

  exit(0L);
}

#endif

/********************************************************************/
/*                                                                  */
/********************************************************************/

