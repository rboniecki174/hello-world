/************************************************************************************/
/*                                                                                  */
/* private_data.h   "C" verion of the private_data common block           R00-02.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the definition of the private_data common block.              */
/*                                                                                  */
/* This file contains data that is mirrored in gb1_init.inc (FORTRAN).  Any changes */
/* here must be made there.                                                         */
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
/* This file contains data that is mirrored in gb1_init.inc (FORTRAN).  Any changes */
/* here must be made there.                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        23/Mar/1998  New file                                    */
/* R00-01.00  DRLJr        06/Mar/1999  Added array to support I/O mirroring        */
/* R00-02.00  DRLJr        11/Mar/1999  Changed definitions to use parameters.      */
/* R00-02.01  DRLJr        20/Oct/1999  Found that base_task_name is mistyped.  It  */
/*                                      should be unsiged char of TASKNAMELEN and   */
/*                                      not int     .                               */
// 78919      jeffs        21/Apr/2014  Add structures to hold the file descriptor
//                                      needed for converion of ftn to c opens.
/*                                                                                  */
/************************************************************************************/


/*==================================================================================*/
/* Macro Definitions                                                                */
/*==================================================================================*/

#ifndef __PRIVATE_DATA_H__
#define __PRIVATE_DATA_H__

#include "devicecodes.h"

#define  MAXIMUM_LU       255                   /* Maximum LU number supported      */

#include "process_init.h"

#define  HOSTPORT_PRIMARY    +1
#define  HOSTPORT_NEITHER     0
#define  HOSTPORT_BACKUP     -1

#define  HOSTPORT_CONNECTED  +1
#define  HOSTPORT_DOCONNECT   0
#define  HOSTPORT_RECONNECT  -1


/*==================================================================================*/
/* Private Data Work Common BLock Structure                                         */
/*==================================================================================*/

typedef  struct cmn_private_data {              /* Structure of private_data common */

  /*--------------------------------------------------------------------------------*/
  /* Real Time Task Slot Number.  The slot number is 1 based.  For "C" programs the */
  /* slot value must be offset to a 0 based value.                                  */
  /*--------------------------------------------------------------------------------*/

  int                my_slot;                   /* Slot from the realtime table     */

  /*--------------------------------------------------------------------------------*/
  /* Sysexc Flag (Application is a SYSEXC program)                                  */
  /*--------------------------------------------------------------------------------*/

  int                is_sysexc;                 /* Is Sysexc (BOOLEAN)              */

  /*--------------------------------------------------------------------------------*/
  /* Mapping and Local Data Definitions                                             */
  /*--------------------------------------------------------------------------------*/

  char               task_desk_info[TASKNAMELEN];/* XDPn Task/Display Information   */
  int                number_cmd_args;           /* Number of Command Arguments      */
  int                first_task_arg;            /* First argument for the task      */
  int                base_task_name[TASKNAMELEN];/* Base name from argv[0]          */
  unsigned char      termtype;                  /* Terminal Type                    */
  unsigned char      termtype_tcp;              /* TCP Client or Server             */
  unsigned char      desk;                      /* Desk Identifier                  */
  unsigned char      program;                   /* Program Identifier               */
  int                port;                      /* Port Number                      */
  unsigned char      termid[80];                /* Argument 1 data                  */
  unsigned char      device_name[80];           /* Constructed device name          */

  /*--------------------------------------------------------------------------------*/
  /* Logical Unit Mapping Tables                                                    */
  /*--------------------------------------------------------------------------------*/

  short              lu_to_fd[MAXIMUM_LU+1L];   /* FTN to Unix fd mapping table     */
  unsigned char      lu_is_tcp[MAXIMUM_LU+1L];  /* LU is attached to TCP (BOOLEAN)  */
  unsigned short      lu_dcod[MAXIMUM_LU+1L];    /* Device Types                     */
  int                lu_recl[MAXIMUM_LU+1L];    /* Record length of the files       */
  int                lu_size[MAXIMUM_LU+1L];    /* Number of records                */
  short              lu_mirror[MAXIMUM_LU+1L];  /* Unix Fd to use to mirror I/O     */


  /*--------------------------------------------------------------------------------*/
  /* Primary/Backup Host:Service Strings                                            */
  /*--------------------------------------------------------------------------------*/

  int                hostport_flag;             /* Host/Port Flag (0=Primary)       */
  unsigned char      hostport_name[WRK_NAMESIZE];/* Host/Port for Primary/Backup    */
  int                hostport_delay;            /* Delay between connection attempts*/

  /*--------------------------------------------------------------------------------*/
  /* Filenames associated with logical units                                        */
  /*--------------------------------------------------------------------------------*/

  unsigned char      lu_filename[MAXIMUM_LU+1][WRK_NAMESIZE]; /* Filenames to lus   */

} cmn_private_data;                             /*                                  */

//local globals.
int fnum_store[MAXIMUM_LU+1];     //78919
int recl_store[MAXIMUM_LU+1];     //78919

/*==================================================================================*/
/* Private Data Work Common Block Data Intialization                                */
/*==================================================================================*/

#ifdef BLOCK_DATA           

           cmn_private_data   private_data;      /* Private Data Common Block        */

//out                  = {
//out                              0L,               /* my_slot                          */
//out                              0L,               /* is_sysexc                        */
//out                              {BLANK8},           /* task_desk_info                   */
//out                              0L,               /* number_cmd_args                  */
//out                              0L,               /* first_task_arg                   */
//out                              {BLANK8},           /* base_task_name                   */
//out                              ' ',              /* termtype                         */
//out                              ' ',              /* termtype_tcp                     */
//out                              ' ',              /* desk                             */
//out                              ' ',              /* program                          */
//out                              0L,               /* port                             */
//out                              {BLANK80},          /* termid                           */
//out                              {BLANK80},          /* device_name                      */
//out                              {ZERO256},          /* lu_to_fd                         */
//out                              {ZERO256},          /* lu_is_tcp                        */
//out                              {MINUSONE256},      /* lu_dcod                          */
//out                              {MINUSONE256},      /* lu_recl                          */
//out                              {MINUSONE256},      /* lu_size                          */
//out                              {MINUSONE256},      /* lu_mirror                        */
//out
//out                              {HOSTPORT_NEITHER}, /* hostport_flag                    */
//out                              {ZERO256},          /* hostport_name                    */
//out
//out                              ZERO512_256       /* lu_filename                      */
//out
//out                    } ;                         /* End of Initialization            */

#else

  extern   cmn_private_data   private_data;     /* Private Data Common Block        */

#endif

#endif


/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
