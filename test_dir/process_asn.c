/************************************************************************************/
/*                                                                                  */
/* process_asn.c    "C" version of ProcessAsn common block                R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the definition of the processasn common block.                */
/*                                                                                  */
/* This file contains data that is mirrored in process_asn.inc (FORTRAN).  Any      */
/* changes here must be made there.                                                 */
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
/* This file contains data that is mirrored in process_asn.inc (FORTRAN).  Any      */
/* changes here must be made there.                                                 */
/*                                                                                  */
/* For common block variables that start with the string "Value" a value of -1      */
/* indicates that the keyword is not present.  A value of 0 or greater indicates    */
/* that the keyword is present.                                                     */
/*                                                                                  */
/* For common block variables that start with the string "Flag" a value of 0        */
/* indicates that the keyword is not present and a value of 1 indicates that the    */
/* keyword is present.                                                              */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        26/Mar/1999  Created "C" version of common block.        */
/* R00-01.00  DRLJr        28/Mar/1999  Added initialization of common block.       */
/*                                                                                  */
/************************************************************************************/

#define  BLOCK_DATA    1

#include "process_asn.h"

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/

