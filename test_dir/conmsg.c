/************************************************************************************/
/*                                                                                  */
/* conmsg.c     FORTRAN Console Message Logging Utility                   R01-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This routine is used to write a message to the console or stderr of the caller.  */
/* This routine replaced the original FORTRAN version.                              */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/* conmsg(int      * length, char * text, int      text_len)                        */
/*                                                                                  */
/*   length   - This is the length of the buffer to print.                          */
/*   text     - This is the text to print.  It may be any type.                     */
/*   text_len - This is the length of the character variable text if text is passed */
/*            - as character variable.                                              */
/*                                                                                  */
/*==================================================================================*/
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
/* R00-00.00  DRLJr        25/Mar/1998  File created to replace consmg.f            */
/* R00-01.00  DRLJr        30/Sep/1998  Correct argument list                       */
/* R00-01.01  DRLJr        30/Sep/1998  Insure a newline is put out to emulate the  */
/*                                      way the OS/32 CONMSG() works.               */
/* R01-00.00  DRLJr        30/Sep/1998  Overhaul of the code so that it will get    */
/*                                      prepend the taskname of the task to the     */
/*                                      message being written to standard error.    */
/*            jeffs        28/Jan/2013  Rewrite to use dlog.                        */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include <dlogdefs.h>

void get_tname_(char * name, int      length);


void conmsg_ (int      * length, char * text, int      text_len)
{
   char taskname[8];
   (void) get_tname_(taskname,8L);   
   dLog(DLOG_WARNING,"<%.8s : %.*s>", taskname, *length, text);
}

/*============================================================================*/
/* Test Program                                                               */
/*============================================================================*/

#if __MAIN__

int      main(int      argc, char ** argv)
{
  int      len;
  char     msg[44];
 
  memcpy(msg,"This is a 44 character message (for conmsg).",44L);
  len   = 44L;

  conmsg_(&len,msg,len);

  exit(0L);
}

void get_tname_(char * name, int      length)
{
  memcpy(name,"CONMSGxx=================",length);
  return;
}

#endif

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
