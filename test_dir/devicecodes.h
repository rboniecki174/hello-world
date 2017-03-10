/************************************************************************************/
/*                                                                                  */
/* device_codes.h   "C" definitions of the device codes                   R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the definition of the device codes used in the OS/32 emulation*/
/* subsystem.  Definitions changed here must be reflected in gb1_init.inc (FORTRAN).*/
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
/*                                                                                  */
/************************************************************************************/

#ifndef __DEVICECODES_H__
#define __DEVICECODES_H__

#define DCOD_CONTIG         0           /* OS/32 CONTIGUOUS FILE                    */
#define DCOD_INDEX          2           /* OS/32 INDEXED FILE                       */
#define DCOD_BRKSHRT       34           /* Broker Terminal Device                   */
#define DCOD_SYSEXC        35           /* Sysexc Terminal Device                   */
#define DCOD_MARPAD        36           /* Marpad Terminal Device                   */
#define DCOD_PRINTER      113           /* Printer Device                           */
#define DCOD_TTYM          38           /* General Purpose TTY Device (modified)    */
#define DCOD_TTY           39           /* General Purpose TTY Device               */
#define DCOD_NULL         255           /* NULL Device (Bit Bucket)                 */

#define SECTOR            256           /* Number of bytes in an OS/32 sector       */

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
