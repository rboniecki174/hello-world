/*******************************************************************************/
/*                                                                             */
/* gb1_init.c      Garban Broker System Initialization               R01-00.00 */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Programing Notes:                                                           */
/*                                                                             */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Revision History:                                                           */
/*                                                                             */
/* Revision   Date         Whom      Why                                       */
/* =========  ===========  ========  ========================================= */
/* R00-00.00  06/Apr/1999  DRLJr     Routine adapted from gb1_init.f           */
/* R01-00.00  20/Oct/1999  DRLJr     Moved realtime common setup to a new file */
/*                                   set_realtime.c.                           */
/* R01-00.00 jeffsun    21/Feb/2001  Replace realtime1 with REALTIME1 for sparc  */
// tt60135    03/02/11     GJ        Replace obsolete realtime1.h with gb1_init.h
// tt80093    05/22/2014   gj        Clean up prototypes
/*                                                                             */
/*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "private_data.h"
#include "gb1_init.h"
#include "devicecodes.h"
#include "process_asn.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef LINT
#define LINT   int     
#endif

#ifndef STRMIN
#define STRMIN(x,y) ((x < y) ? x : y)
#endif

#define MY_SLOT           private_data.my_slot
#define CMY_SLOT         (private_data.my_slot - 1L)
#define BASE_TASK_NAME    (UCHAR *) private_data.base_task_name
#define SHARED_NAMES      REALTIME1.shared_names

/*=============================================================================*/
/* Prototypes for FORTRAN Functions and other functions                        */
/*=============================================================================*/

int              iargc_(void);
void             getarg_(int      * argno, unsigned char * str, 
                         int        strln);

void             process_asn_(int      * status);
int              get_slot_(unsigned char * name, int      name_len);
int              dbg_queue_state(void);
void             set_signals_(void);
//void dump_copen_store(void);    //78918 for debug

/*=============================================================================*/
/* gb1_init (FORTRAN Interface)                                                */
/*=============================================================================*/

int        gb1_init_(int      * Lu1, int      * Lu2)
{
  int              status;                    /* General working status        */
  int              zero;                      /* A general working zero value  */
  int              index;                     /* Working index                 */
  char    workname[256];             /* Allow 256 bytes               */
  unsigned char    taskname[8];               /* Working taskname              */
  unsigned char    empty[8];                  /* Working taskname              */
  char  * pntr;                      /* Working Pointer               */

  /*===========================================================================*/
  /* Do the basic initialization processing                                    */
  /*===========================================================================*/

  IF_DEBUG(1)
    fprintf(stderr,"gb1_init - Initializing\n");
  END_DEBUG

  zero                            = 0L;
  status                          = 0L;
  MY_SLOT                         = 0L;
  private_data.number_cmd_args    = iargc_();
  private_data.first_task_arg     = 1L;

  memset(private_data.task_desk_info,(int     ) ' ', 8L);
  memset(taskname,                   (int     ) ' ', 8L);
  memset(empty,                                  0L, 8L);
  
  /*===========================================================================*/
  /* Set the initial version of the taskname                                   */
  /*===========================================================================*/

  IF_DEBUG(1)
    fprintf(stderr,"gb1_init - Settng Default Taskname\n");
  END_DEBUG

  getarg_(&zero,(unsigned char *)workname,256L-1L);
  workname[256L-1L] = '\0';
  
  pntr  = strrchr(workname,'/');
  if(pntr == (char *) NULL) {
    pntr = workname;
  }
  else {
    pntr++;
  }

  for(index=0;index<8;index++,pntr++) {
    if((*pntr) == '\0') {
      break;
    }
    taskname[index] = (*pntr);
  }
  memcpy(private_data.base_task_name,taskname,8L);

  /*===========================================================================*/
  /* Process the run file                                                      */
  /*===========================================================================*/

  IF_DEBUG(1)
    fprintf(stderr,"gb1_init - Calling process_asn_() \n");
  END_DEBUG
  process_asn_(&status);
  if(status < 0L) {
    exit(254L);
  }
  IF_DEBUG(1)
    fprintf(stderr,"gb1_init - Returned from process_asn_()\n");
  END_DEBUG

  memcpy(private_data.task_desk_info,private_data.base_task_name,8L);
  
  private_data.first_task_arg++;

  //dump_copen_store();    // 78918  for debug
  /*===========================================================================*/
  /* Set up the realtime common block with the taskname                        */
  /*===========================================================================*/


  status = set_realtime_();


  /*===========================================================================*/
  /* Return to the caller                                                      */
  /*===========================================================================*/

  status = 0L;
  return(status);
}

/*******************************************************************************/
/*                                                                             */
/*******************************************************************************/
