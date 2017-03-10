/********************************************************************************/
/*                                                                              */
/* qheaders.h   OS/32 Circular List Header and Structure Format       R00-01.00 */
/*                                                                              */
/*==============================================================================*/
/*                                                                              */
/* Programming Notes                                                            */
/*                                                                              */
/* The circular list access routines use the high order bit of the S_AVAIL      */
/* field as a access control lock.  The number of slots allowed in the list is  */
/* not 65535 as with OS/32 but 32767 due to the use of this bit.                */
/*                                                                              */
/*==============================================================================*/
/*                                                                              */
/* Revision History                                                             */
/*                                                                              */
/* Revision   When         Who       Why                                        */
/* =========  ===========  ========  ========================================== */
/* R00-00.00  02/Oct/1998  DRLJr     Created "C" version of qheaders.inc        */
/* R00-01.00  21/Apr/1999  DRLJr     Convert prototypes to used standard queue  */
/*                                   header names.                              */
// tt80093    18/Jun/2014  gj        Clean up prototypes
/*                                                                              */
/********************************************************************************/

#ifndef __QHEADERS_H__
#define __QHEADERS_H__  1

#define S_AVAIL    0            /* Number of slots in the list                 */
#define S_USED     1            /* Number of slots used in the list            */
#define S_TOP      2            /* The top slot used in the list               */
#define S_BOT      3            /* The next botton slot in the list            */

#define S_AVAIL4   0            /* Number of slots in the list                 */
#define S_USED4    1            /* Number of slots used in the list            */
#define S_TOP4     2            /* The top slot used in the list               */
#define S_BOT4     3            /* The next botton slot in the list            */

// tt80093 START
//#include "list.h"

//#if 0
//typedef struct cirlist {
//          unsigned        : 0;
//          unsigned short  nslots;
//          unsigned short  nused;
//          unsigned short  curtop;
//          unsigned short  curbot;  /* Note: This is next Bottom    */
//} cirlist_t;

//int        getlisthdr(int      field, cirlist_t * list);
//void       getlisthdr2_(short      * field, cirlist_t * list, short      * value);
//void       getlisthdr4_(int       * field, cirlist_t * list, int       * value);
//#endif

//int        getlisthdr(int      field, cirlist * list);
//void       getlisthdr2_(short      * field, cirlist * list, short      * value);
//void       getlisthdr4_(int       * field, cirlist * list, int       * value);
// tt80093 END

#endif

/********************************************************************************/
/*                                                                              */
/********************************************************************************/
