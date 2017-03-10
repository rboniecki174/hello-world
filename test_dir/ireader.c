/********************************************************************/
/*                                                                  */
/* ireader.c   Perform an IREADER Command                 R00-00.00 */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Programing Notes:                                                */
/*                                                                  */
/* This routine is a replacement for the ireader.f subroutine.      */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* Revision History:                                                */
/*                                                                  */
/* Revision   Date         Whom      Why                            */
/* =========  ===========  ========  ============================== */
/* R00-00.00  05/Apr/1999  DRLJr     Replacement for ireader.f      */
/*                                                                  */
/********************************************************************/

#include <stdlib.h>
#include <string.h>

#define  BUFR_SIZE    512L

/*==================================================================*/
/* Ireader() ("C" Interface)                                        */
/*==================================================================*/

int      Ireader(unsigned char * buffer)
{
  int              status;               /* Status Variable         */

  status         = system((char *)buffer);       /* Invoke the command      */
  return(status);                        /* Return status to caller */
}                                        /*                         */

/*==================================================================*/
/* Ireader() (FORTRAN Interface)                                    */
/*==================================================================*/

void ireader_(unsigned char * buffer, int      * length,
              int           * status, int        buffer_len)
{
  unsigned char        wbuffr[BUFR_SIZE];/* Working Buffer          */
  int                  wlength;          /* Working Length          */
  
  wlength = (((*length) < BUFR_SIZE) ? (*length) : BUFR_SIZE - 1L);

  memset(wbuffr,0x00,BUFR_SIZE);         /* Clear the buffer space  */
  memcpy(wbuffr,buffer,wlength);         /* Copy the text           */

  (*status) = Ireader(wbuffr);           /* Call the "C" interface  */

  return;                                /* Return to the caller    */

}                                        /*                         */

/********************************************************************/
/*                                                                  */
/********************************************************************/
