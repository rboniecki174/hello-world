/*******************************************************************************/
/*                                                                             */
/* process_openlu.c    Open a FORTRAN Logical Unit                   R00-00.01 */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Programing Notes:                                                           */
/*                                                                             */
/* This code is included in process_asnc.c and process_asnf.c                  */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Revision History:                                                           */
/*                                                                             */
/* Revision   Date         Whom      Why                                       */
/* =========  ===========  ========  ========================================= */
/* R00-00.00  06/Apr/1999  DRLJr     Routine adapted from process_asn.f        */
/* R00-00.01  19/May/1999  DRLJr     Put logical unit into the process_asn     */
/*                                   working common block (process_asn).       */
/*                                                                             */
/*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "process_asn.h"
#include "devicecodes.h"

#ifndef LINT
#define LINT   int     
#endif

#ifndef STRMIN
#define STRMIN(x,y) ((x < y) ? x : y)
#endif

/*=============================================================================*/
/* openlu() (FORTRAN Interface)                                                */
/*=============================================================================*/

#ifdef C_INTERFACE
void OpenLu
#endif
#ifdef FTN_INTERFACE
void openlu_
#endif
            (
             LINT   * Lu,
             UCHAR  * File, 
             LINT   * Dcode, 
             LINT   * SizeRecl,
             UCHAR  * FTN_Status, 
             UCHAR  * FTN_Access,
             UCHAR  * FTN_Form,
             LINT   * FTN_Recl,
             LINT   * FTN_Readonly,
             LINT   * FTN_Renew,
             LINT   * status,
             LINT     File_Len,
             LINT     FTN_Status_Len,
             LINT     FTN_Access_Len,
             LINT     FTN_Form_Len)
{
  
  /*===========================================================================*/
  /* Clear the working common block                                            */
  /*===========================================================================*/

  Process_ASN_Clear_Common();             

  /*===========================================================================*/
  /* Indicate we are openning a LOCAL device or file                           */
  /*===========================================================================*/

  processasn.Flag_LOCAL  = 1L;

  /*===========================================================================*/
  /* Set Basic Options based upon the Device Code                              */
  /*===========================================================================*/

  if((*Dcode) == DCOD_CONTIG) {
    processasn.Flag_CO  =  1L;
    processasn.Value_CO = (*SizeRecl);
  }
  else if((*Dcode) == DCOD_INDEX) {
    processasn.Flag_IN  =  1L;
    processasn.Value_IN = (*SizeRecl);
  }
  else {
    fprintf(stderr,"Device Code %d is not suppored by openlu().\n",(*Dcode));
  }

  /*===========================================================================*/
  /* Move the options to the common block for processing                       */
  /*===========================================================================*/

  processasn.Value_Lu           = (*Lu);

  memcpy(processasn.Value_Filename, File,      STRMIN(File_Len,     256L));
  memcpy(processasn.FTN_OPEN_Status,FTN_Status,STRMIN(FTN_Status_Len,12L));
  memcpy(processasn.FTN_OPEN_Access,FTN_Access,STRMIN(FTN_Access_Len,12L));
  memcpy(processasn.FTN_OPEN_Form  ,FTN_Form  ,STRMIN(FTN_Form_Len,12L));

  processasn.FTN_OPEN_Recl      = (*FTN_Recl);
  processasn.FTN_OPEN_Readonly  = (*FTN_Readonly);
  processasn.FTN_OPEN_Renew     = (*FTN_Renew);

  /*===========================================================================*/
  /* Go and Assign the file                                                    */
  /*===========================================================================*/

  (*status)  = AttachLU();

  return;
}

/*******************************************************************************/
/*                                                                             */
/*******************************************************************************/
