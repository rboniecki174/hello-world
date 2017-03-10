/************************************************************************************/
/*                                                                                  */
/* iclock.c     Get the Time from the system clock                        R00-01.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
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
/* R00-00.00  DRLJr        16/Apr/1999  Added header for documentation and made     */
/*                                      various structural changes.  Made code      */
/*                                      conform to the proper definition.           */
/* R00-01.00  DRLJr        07/Mar/2000  Added function 3 which returns the number   */
/*                                      of seconds since midnight of 01/Jan/1970.   */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include "iclock.h"      
#include <dlogdefs.h>

void iclock_(unsigned int      * fun,unsigned int      * list)
{
  int          count;
  time_t       my_time;
  struct tm  * my_tm;
  char       * asciitime;
  char       * inptr;
  char       * outptr;


  switch ((*fun)) { /* Perform the operation based upon function code ............. */

    case 0 :        /* Get the time as three integers (hours, minutes, seconds)     */
                    my_time   = time(NULL);
                    my_tm     = localtime(&my_time);
                    *(list++) = my_tm->tm_hour;
                    *(list++) = my_tm->tm_min;
                    *(list)   = my_tm->tm_sec;
                    break;

    case 1 :        /* Get time as eight byte format string (hh:mm:ss)              */
                    my_time   = time(NULL);
                    my_tm     = localtime(&my_time);
                    asciitime = asctime(my_tm);
                    inptr     = asciitime + 11;
                    outptr    = (char *) list;
                    for(count=8;count;count--,inptr++,outptr++) {
                      *outptr = *inptr;
                    }
                    break;

    case 2 :        /* Get time as number of seconds since midnight                 */
                    my_time   = time(NULL);
                    my_tm     = localtime(&my_time);
                    *list     = (3600 * my_tm->tm_hour) +
                                (  60 * my_tm->tm_min)  + 
                                (       my_tm->tm_sec);
                    break;

    case 3 :        /* Get time as number of seconds since midnight January 1, 1970 */
                    my_time   = time(NULL);
                    (*list)   = (int     ) my_time;
                    break;

    default :       /* The function code (*fun) is not a valid value                */
                    dLog(DLOG_MAJOR,"iclock.c - Illegal ICLOCK function (%d)",(*fun));
                    break;
  }

  return;
}


/*==================================================================================*/
/* Test Program                                                                     */
/*==================================================================================*/

#ifdef MAIN

int      main(int      argc, char ** argv)
{
  unsigned int          bin_hms[3];
  unsigned char         asc_hms[12];
  unsigned int          midnight;
  unsigned int          func;
  
  func     = 0L;  iclock_(&func, bin_hms);
  dLog(DLOG_STDLOG,"func = %4d, Result = %2.d-%2.d-%2.d \n",
                 func,bin_hms[0],bin_hms[1],bin_hms[2]);

  func     = 1L;  iclock_(&func,(unsigned int      *) &asc_hms);
  dLog(DLOG_STDLOG,"func = %4d, Result = %8.8s \n",func,asc_hms);

  func     = 2L;  iclock_(&func,&midnight);
  dLog(DLOG_STDLOG,"func = %4d, Result = %8d \n",func,midnight);

  exit(0L);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
