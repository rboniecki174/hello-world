/************************************************************************************/
/*                                                                                  */
/* process_init.h   Common MACRO used by process_asn et. all              R01-00.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
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
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        13/Apr/1999  Created file from process_asn.h             */
/* R01-00.00  DRLJr        ??/Apr/1999  Added various definitions                   */
/* R01.00.01  DRLJr        22/Apr/1999  Added MINUSONE32                            */
/*                                                                                  */
/************************************************************************************/

#ifndef __PROCESSINIT_H__  
#define __PROCESSINIT_H__

#ifndef  FALSE
#define  FALSE              0                   /* Define a False value             */
#endif

#ifndef  TRUE
#define  TRUE               1                   /* Define a True value              */
#endif

#define  MAXIMUM_LU       255                   /* Maximum LU number supported      */

#ifndef  UCHAR
#define  UCHAR     unsigned char
#endif

#ifndef  TASKNAMELEN
#define  TASKNAMELEN        8
#endif

#define  MDEQTRNS_LU1      10                   /* Read LU for MDEQTRNS             */
#define  MDEQTRNS_LU2      11                   /* Write LU for MDEQTRNS            */

#define  BRKSHRT_LU1       20                   /* Read LU for BRKSHRT              */
#define  BRKSHRT_LU2       22                   /* Write LU for BRKSHRT             */

#define  OPEN_READ_LU     255                   /* LU For RUN FILES                 */

#define  RUN_DEPTH          4                   /* Number of nested RUN Files       */

#define  WRK_NAMESIZE     512                   /* Size of the work/name strings    */

#define BLANK4    ' ',' ',' ',' '
#define BLANK8     BLANK4,BLANK4
//#define BLANK12    BLANK4,BLANK4,BLANK4
#define BLANK40    BLANK8,BLANK8,BLANK8,BLANK8,BLANK8
//#define BLANK64    BLANK8,BLANK8,BLANK8,BLANK8,BLANK8,BLANK8,BLANK8,BLANK8
#define BLANK80    BLANK40,BLANK40
//#define BLANK512   BLANK64,BLANK64,BLANK64,BLANK64,BLANK64,BLANK64,BLANK64,BLANK64

//#define ZERO16       0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
//#define ZERO64       ZERO16,ZERO16,ZERO16,ZERO16
//#define ZERO256      ZERO64,ZERO64,ZERO64,ZERO64
//#define ZERO512      ZERO256,ZERO256
//#define ZERO4_256    ZERO256,ZERO256,ZERO256,ZERO256
//#define ZERO16_256   ZERO4_256,ZERO4_256,ZERO4_256,ZERO4_256
//#define ZERO64_256   ZERO16_256,ZERO16_256,ZERO16_256,ZERO16_256
//#define ZERO256_256  ZERO64_256,ZERO64_256,ZERO64_256,ZERO64_256
//#define ZERO512_256  ZERO256_256,ZERO256_256


#define MINUSONE16    -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1
#define MINUSONE64    MINUSONE16,MINUSONE16,MINUSONE16,MINUSONE16
#define MINUSONE256   MINUSONE64,MINUSONE64,MINUSONE64,MINUSONE64

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/

