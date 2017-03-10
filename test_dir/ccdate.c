/************************************************************************************/
/*                                                                                  */
/* ccdate.c     Emulation of OS/32 FORTRN VII RTL DATE Subroutine         R02-00.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines to perform a sleep operation based upon the      */
/* arguments that are passed to the routine.                                        */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/* DateArr - This is the location of 3 word integer*4 array to return the local     */
/*         - date (use local date & time information to insure correct results).    */
/*         -                                                                        */
/*         - Array element 1 (1 based) is the year (0-99).                          */
/*         - Array element 2 (1 based) is the month (1-12).                         */
/*         - Array element 3 (1 based) is the day (1-31).                           */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*  These routines are intended to be called from FORTRAN code or "C" code that     */
/*  acts like FORTRAN code.                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R02-00.00  DRLJr        01/Apr/1998  FORTRAN converted to "C".                   */
/* R02-00.01  DRLJr        28/Dec/1999  Insure year is always in the range 00-99.   */
/*                                      localtime[_r]() returns the number of years */
/*                                      since 1900 not the year in the century.     */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dlogdefs.h>


#define  USE_LOCALTIME_R   1

void ccdate_(int      * datearr)
{
  struct tm       tdate;
  struct tm     * ptrtm;
  time_t          local_time;

  local_time      = time((time_t *) NULL);
#if (USE_LOCAL_TIME_R == 1)
  ptrtm           = (struct tm *) localtime_r(&local_time,&tdate); 
#else
  ptrtm           = (struct tm *) localtime(&local_time); 
  (void)            memcpy(&tdate,ptrtm,sizeof(struct tm));
#endif

  tdate.tm_mon++;                        /* Convert month from 0 - 11 to 1 - 12     */

  datearr[0]      = tdate.tm_year;
  datearr[0]     %= 100L;                /* Keep in the 00-99 range (R02-00.01)     */
  datearr[1]      = tdate.tm_mon;
  datearr[2]      = tdate.tm_mday;

#ifdef MAIN
   dLog(DLOG_STDLOG," %02.2d:%02.2d:%02.2d ",tdate.tm_hour,tdate.tm_min,tdate.tm_sec);
   dLog(DLOG_STDLOG," %02.2d/%02.2d/%02.2d ",tdate.tm_mday,tdate.tm_mon,tdate.tm_year);
   dLog(DLOG_STDLOG,"  Day:%2d (0=Sunday), DOY:%4d, DLST:%2d",
                    tdate.tm_wday,tdate.tm_yday,tdate.tm_isdst);
#endif

  return;
}

#ifdef MAIN

int      main(int      argc, char ** argv)
{
  int         curdate[3];

  ccdate_(curdate);

   dLog(DLOG_STDLOG," Date (DD/MM/YY): %02.2d/%02.2d/%02.2d",
          curdate[2],curdate[1],curdate[0]);

  exit(0);
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
