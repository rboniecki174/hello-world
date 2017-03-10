/*******************************************************************************/
/*                                                                             */
/* gb1_initc.c     Garban Broker System Initialization               R01-00.00 */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Programing Notes:                                                           */
/* C interface verson required for __386 calling                               */
/*                                                                             */
/* =========================================================================== */
/*                                                                             */
/* Revision History:                                                           */
/*                                                                             */
/* Revision   Date         Whom      Why                                       */
/* =========  ===========  ========  ========================================= */
/* R01-00.00  20/Aug/2008  jeffs     Replace gb1_init.c for sparc to amd       */
// tt60135    03/02/11     GJ        Replace obsolete realtime1.h with gb1_init.h
// tt80093    05/22/14     gj        Clean up prototypes
/*                                                                             */
/*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

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
void             getarg_(int * argno, unsigned char * str,int strln);
void             process_asn_c(char *BaseName, int * status);
int              dbg_queue_state(void);
void             set_signals_(void);
// void dump_copen_store(void);    // 78918  for debug

/*=============================================================================*/
/* gb1_init (FORTRAN Interface)                                                */
/*=============================================================================*/

int       gb1_initc(int argc, char** argv)
{
  int              status;                    /* General working status        */
  int              zero;                      /* A general working zero value  */
  int              index;                     /* Working index                 */
  unsigned char    taskname[8];               /* Working taskname              */
  unsigned char    empty[8];                  /* Working taskname              */
  char  * pntr;                      /* Working Pointer               */
  char *workname = argv[0];
  char *BaseName;


  /*===========================================================================*/
  /* Do the basic initialization processing                                    */
  /*===========================================================================*/
  if (argc == 1)
  {   
      fprintf(stderr,"ERROR: No RUN-FILE basename on the command line.\n");
      exit(254);
  } 
  BaseName = argv[1];

  fprintf(stdout,"gb1_initc - Initializing\n");

  zero                            = 0L;
  status                          = 0L;
  MY_SLOT                         = 0L;
  private_data.number_cmd_args    = argc;
  private_data.first_task_arg     = 1L;

  memset(private_data.task_desk_info,(int     ) ' ', 8L);
  memset(taskname,                   (int     ) ' ', 8L);
  memset(empty,                                  0L, 8L);
  
  /*===========================================================================*/
  /* Set the initial version of the taskname                                   */
  /*===========================================================================*/
  pntr              = strrchr(workname,'/');
  if(pntr == NULL) {
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

  fprintf(stdout,"gb1_initc - Settng Default Taskname <%.8s>\n",taskname);
  /*===========================================================================*/
  /* Process the run file                                                      */
  /*===========================================================================*/
  fprintf(stderr,"gb1_init - Calling process_asn_() with basename <%.8s> \n", BaseName);

  process_asn_c(BaseName,&status);

  if(status < 0L) {
    exit(254L);
  }
  IF_DEBUG(1)
    fprintf(stderr,"gb1_init - Returned from process_asn_()\n");
  END_DEBUG

  memcpy(private_data.task_desk_info,private_data.base_task_name,8L);
  
  private_data.first_task_arg++;

//  dump_copen_store();    // 78918  for debug
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
