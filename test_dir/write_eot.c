/**********************************************************/
/*                                                        */
/* write_eot - Write Termination Message                  */
/*                                                        */
/* Generic Program Termination Message                    */
/*                                                        */
/* 11/Jan/1998 DRLJr Changed code to accept taskname as   */
/*                   an argument to allow for names that  */
/*                   are greater than 8 bytes in length   */
/*                                                        */
/**********************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <dlogdefs.h>

static   char   time_unknown[] =
                 { "<Unable to get date & time>" };

#if 0
void get_tname_(char *, int     );
#endif

void write_eot(char * taskname)
{
  time_t     datetime;
  int        status;
  char     * cdatetime;
  static int  wrote_once = 0;

  if (wrote_once) return;   /* this avoids duplicate write under the atexit handler */

  status  = time(&datetime);
  if(status < 0) {
    cdatetime = time_unknown;
  }
  else {
    cdatetime = ctime(&datetime);
    status    = strlen(cdatetime);
    if(cdatetime[status-1] == '\n') {
      cdatetime[status-1] = ' ';
    }
  }
  if (dLogTypes[DLOG_STDLOG].fd != stdout)
  {
    // Avoid duplicate if logging goest to stdout also. //
    dLog(DLOG_STDLOG," %-.8s : Task Terminated on %s",taskname,cdatetime); 
  }
  dLog(DLOG_STDOUT," %-.8s : Task Terminated on %s",taskname,cdatetime); 
  wrote_once = 1;

  return;
}

/**********************************************************/
/*                                                        */
/**********************************************************/

