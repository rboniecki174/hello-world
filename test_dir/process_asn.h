/************************************************************************************/
/*                                                                                  */
/* process_asn.h    "C" verion of ProcessAsn common block                 R01-00.00 */
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
/* R01-00.00  DRLJr        01/Apr/1999  Added flags for FORTRAN options             */
/*                                                                                  */
/************************************************************************************/

#ifndef __PROCESSASN_H__  
#define __PROCESSASN_H__  

#include "process_init.h"

#define  IF_DEBUG(x)  if((processasn.Debug_Flag > 0)&&(x <= processasn.Debug_Flag)) {
#define  END_DEBUG    }


/*==================================================================================*/
/* Command Numbers (Must match table order in process_cmds.c)                       */
/*==================================================================================*/

#define CMD_ASSIGN             0
#define CMD_TASK               1
#define CMD_DEBUGQUEUE         2
#define CMD_CALL               3
#define CMD_DATA_PATH          4
#define CMD_REPORT_PATH        5
#define CMD_MIRROR             6
#define CMD_PRIMARY            7
#define CMD_BACKUP             8
#define CMD_DEBUG              9

/*==================================================================================*/
/* Assign Options  (Must match table order in process_cmds.c)                       */
/*==================================================================================*/

#define ASN_CON                0
#define ASN_PR                 1
#define ASN_CO                 2
#define ASN_IN                 3
#define ASN_BRKSHRT            4
#define ASN_SYSEXC             5
#define ASN_MARPAD             6
#define ASN_RANDOM             7
#define ASN_SEQUENTIALLY       8
#define ASN_TTY                9
#define ASN_LOG               10
#define ASN_CLIENT            11
#define ASN_SERVER            12
#define ASN_TCP               13
#define ASN_LOCAL             14
#define ASN_RECL              15
#define ASN_TIE               16
#define ASN_TTYM              17
#define ASN_MIRROR            18

#define FTN_OLD               19
#define FTN_NEW               20
#define FTN_SCRATCH           21
#define FTN_UNKNOWN           22
#define FTN_APPEND            23
#define FTN_SEQUENTIAL        24
#define FTN_DIRECT            25
#define FTN_FORMATTED         26
#define FTN_UNFORMATTED       27
#define FTN_PRINT             28
#define FTN_READONLY          29
#define FTN_RENEW             30

/*==================================================================================*/
/* General Macros and #define values                                                */
/*==================================================================================*/

#define TCP_CLIENT_RETRY      10            /* On Client Attaches try upto n times  */
#define TCP_CLIENT_DELAY    2000            /* Number of milliseconds between tries */


/*==================================================================================*/
/* Process Assignment Work Common Block Structure                                   */
/*==================================================================================*/

typedef  struct cmn_processasn {                /* Structure of processasn common   */

  /*--------------------------------------------------------------------------------*/
  /* Various Values for the General Purpose Assignment Keywords                     */
  /*--------------------------------------------------------------------------------*/

  int                      Value_CO;            /*                                  */
  int                      Value_IN;            /*                                  */
  int                      Value_DCOD;          /*                                  */
  int                      Value_Lu;            /*                                  */
  int                      Value_Lu2;           /*                                  */
  int                      Value_Tie;           /*                                  */
  int                      Value_Recl;          /* Set from options                 */

  /*--------------------------------------------------------------------------------*/
  /* Only one of the following flags may be set for any one assignment command      */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_CO;             /*                                  */
  int                      Flag_IN;             /*                                  */
  int                      Flag_BRKSHRT;        /*                                  */
  int                      Flag_SYSEXC;         /*                                  */
  int                      Flag_MARPAD;         /*                                  */
  int                      Flag_PR;             /*                                  */
  int                      Flag_CON;            /*                                  */
  int                      Flag_LOG;            /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Only one of the following flags may be set for any one assignment command      */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_CLIENT;         /*                                  */
  int                      Flag_SERVER;         /*                                  */
  int                      Flag_LOCAL;          /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Activate MIRRORing for the particular assignment                               */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_MIRROR;         /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Activate LU Tieing                                                             */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_Tie;            /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Primary/Backup Communication                                                   */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_Primary;        /*                                  */
  int                      Flag_Backup;         /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Flags to indicate if various PATHs are set                                     */
  /*--------------------------------------------------------------------------------*/

  int                      Value_Data_Path;     /* Set from options                 */
  int                      Value_Rept_Path;     /* Set from options                 */
  int                      Value_Mirr_Path;     /* Set from options                 */

  /*--------------------------------------------------------------------------------*/
  /* Only one of the following flags may be set for any one assignment command      */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_FTN_OLD;        /*                                  */
  int                      Flag_FTN_NEW;        /*                                  */
  int                      Flag_FTN_SCRATCH;    /*                                  */
  int                      Flag_FTN_UNKNOWN;    /*                                  */
  int                      Flag_FTN_APPEND;     /*                                  */
  int                      Flag_FTN_RENEW;      /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Only one of the following flags may be set for any one assignment command      */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_FTN_SEQUENTIAL; /*                                  */
  int                      Flag_FTN_DIRECT;     /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* Only one of the following flags may be set for any one assignment command      */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_FTN_FORMATTED;  /*                                  */
  int                      Flag_FTN_UNFORMATTED;/*                                  */
  int                      Flag_FTN_PRINT;      /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* The following flags are used to detirmine if the string has been set           */
  /*--------------------------------------------------------------------------------*/

  int                      Flag_FTN_STATUS;     /*                                  */
  int                      Flag_FTN_ACCESS;     /*                                  */
  int                      Flag_FTN_FORM;       /*                                  */
  int                      Flag_FTN_RECL;       /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* FORTRAN OPEN Parameter Definitions                                             */
  /*--------------------------------------------------------------------------------*/

  unsigned char            FTN_OPEN_Status[12]; /*                                  */
  unsigned char            FTN_OPEN_Access[12]; /*                                  */
  unsigned char            FTN_OPEN_Form[12];   /*                                  */
  int                      FTN_OPEN_Recl;       /*                                  */
  int                      FTN_OPEN_Readonly;   /*                                  */
  int                      FTN_OPEN_Renew;      /*                                  */

  /*--------------------------------------------------------------------------------*/
  /* The file or device that is being assigned                                      */
  /*--------------------------------------------------------------------------------*/

  unsigned char            Value_Filename[WRK_NAMESIZE]; /*                         */

  /*--------------------------------------------------------------------------------*/
  /* The host and port device for primary and backup communication                  */
  /*--------------------------------------------------------------------------------*/

  unsigned char            Value_HostPort[WRK_NAMESIZE]; /*                         */
  int                      Value_HostPort_Delay;         /*                         */

  /*--------------------------------------------------------------------------------*/
  /* Directory Path Values                                                          */
  /*--------------------------------------------------------------------------------*/

  unsigned char            OPEN_DATA_PATH[WRK_NAMESIZE]; /*                         */
  unsigned char            OPEN_REPT_PATH[WRK_NAMESIZE]; /*                         */
  unsigned char            OPEN_MIRR_PATH[WRK_NAMESIZE]; /*                         */

  /*--------------------------------------------------------------------------------*/
  /* RUN-FILE to be open'ed for additional commands                                 */
  /*--------------------------------------------------------------------------------*/

  int                      RunDepth;            /* Current Run Depth                */
  unsigned char            CALL_FileName[RUN_DEPTH][WRK_NAMESIZE];  /*              */

  /*--------------------------------------------------------------------------------*/
  /* Invoked Flag for I/O subsystem                                                 */
  /*--------------------------------------------------------------------------------*/

  int                      Invoked;             /*                                  */

  int                      Debug_Flag;          /*                                  */

} cmn_processasn;                               /*                                  */


/*==================================================================================*/
/* Definition of Process Assignment Work Common Block                               */
/*==================================================================================*/

#ifdef BLOCK_DATA
           cmn_processasn     processasn;        /* ProcessAsn Common Block          */
#else
  extern   cmn_processasn     processasn;       /* ProcessAsn Common BLock          */
#endif

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/

