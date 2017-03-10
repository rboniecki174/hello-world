/************************************************************************************/
/*                                                                                  */
/* check_invoked.c     Check to Insure RUN-FILE processing called         R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines that are used to support the OS/32 sub-system    */
/* and the RUN-FILE assignment mechanism.                                           */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* The "C" version of each function/subroutine is first and the FORTRAN interface   */
/* function/subroutine is second.  The FORTRAN interface function/subroutine        */
/* processes the arguments into a "C" function call.                                */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        06/Apr/1999  Check_Invoked move to its own file          */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include Files                                                                    */
/*==================================================================================*/

#include <stdio.h>

#include "process_asn.h"
#include <dlogdefs.h>


/*==================================================================================*/
/* Global Work Variables                                                            */
/*==================================================================================*/

static char fmt10290[]="************************************************************\n";
static char fmt10291[]="**                                                        **\n";
static char fmt10292[]="**    GB1_INIT/Process_Asn have not be called.            **\n";

/*==================================================================================*/
/* Check_Invoked()                                                                  */
/*                                                                                  */
/* This routine is used to detirmine if the process_asn system has been called to   */
/* perform the needed initialization.                                               */
/*                                                                                  */
/*==================================================================================*/

int      Check_Invoked()                            /* "C" Callable                 */
{
  if(processasn.Invoked == 0L) {                    /* Has process_asn been called  */
    dLog(DLOG_MAJOR,fmt10290);                       /*                              */
    dLog(DLOG_MAJOR,fmt10290);                       /*                              */
    dLog(DLOG_MAJOR,fmt10291);                       /*                              */
    dLog(DLOG_MAJOR,fmt10291);                       /*                              */
    dLog(DLOG_MAJOR,fmt10292);                       /*                              */
    dLog(DLOG_MAJOR,fmt10291);                       /*                              */
    dLog(DLOG_MAJOR,fmt10291);                       /*                              */
    dLog(DLOG_MAJOR,fmt10290);                       /*                              */
    dLog(DLOG_MAJOR,fmt10290);                       /*                              */
  }                                                 /*                              */
  return(processasn.Invoked);                       /* Return the value             */
}                                                   /*                              */

int      check_invoked_()                           /* Fortran Callable             */
{
  return(Check_Invoked());                          /* Return Check_Invoked() value */
}                                                   /*                              */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
