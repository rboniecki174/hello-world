/************************************************************************************/
/* setup_runfile_transaction_log.c                                        R00-00.00 */
/* ================================================================================ */
/* Programming Notes:                                                               */
/*                                                                                  */
/* Will open a stream and if a threshold is reached it will rotate the file.        */
/* Assume that the file filename has been opened in the RUN-FILE.                   */
/* ================================================================================ */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  jeffs        25/Jun/2009  TT 47219 File created                       */
// tt80093    gj           06/Jun/2014  tt80093 Clean up prototypes
/************************************************************************************/

/*==================================================================================*/
/* System Include Files                                                             */
/*==================================================================================*/
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlogdefs.h>
#include <getCurrentDateTimeLog.h>

#include "proto_common.h" //tt80093

/*==================================================================================*/

//void Get_Lu_FD(int Lu, int * YesNo, int * UnixFd);                       //tt80093
//void Get_Lu_Filename(int Lu, unsigned char * UnixFileName);              //tt80093

//void openAndMaybeRotate(const char *filename, FILE **fd, int threshold); //tt80093
//void setup_runfile_log(FILE **fd, int lu, int threshold);                //tt80093
//void open_file_from_runfile_(FILE **fd, int lu);                         //tt80093

/*==================================================================================*/
/* Execution Code                                                                   */
/*==================================================================================*/
// fortran interface
void setup_runfile_log_(FILE **fd, int *lu, int *threshold)
{
   setup_runfile_log(fd, *lu, *threshold);
}

void setup_runfile_log(FILE **fd, int lu, int threshold)
{
   char filename[80];
   int unixfd;
   int dummyzero = 0;

   Get_Lu_Filename(lu, (unsigned char *)filename); // get db name from run file
   if ( *filename == 0x00)
   {
       //NO NAME ASSOCIATED WITH THAT LU
       dLog(DLOG_WARNING,"No Transaction Filename for LU %d.",lu);
       *fd = NULL; 
   }
   else 
   {
       dLog(DLOG_DEBUG_8,"OK, Filename for LU %d is <%s>.",lu, filename);
       //there is an open file. Get the file descriptor and close it.
       Get_Lu_FD(lu, &dummyzero, &unixfd);
       close(unixfd);
       //reopen as a stream for fprintf
       openAndMaybeRotate((char *)filename, fd, threshold);
   }
}


void open_file_from_runfile_(FILE **fd, int lu)
{
   char filename[80];
   int unixfd;
   int dummyzero = 0;

   Get_Lu_Filename(lu, (unsigned char *)filename); // get db name from run file
   if ( *filename == 0x00)
   {
       //NO NAME ASSOCIATED WITH THAT LU
       dLog(DLOG_MINOR,"No Filename for LU %d. FD will be NULL",lu);
       *fd = NULL; 
   }
   else 
   {
       dLog(DLOG_DEBUG_8,"OK, Filename for LU %d is <%s>.",lu, filename);
       //there is an open file. Get the file descriptor and close it.
       Get_Lu_FD(lu, &dummyzero, &unixfd);
       close(unixfd);
       //reopen for read
      *fd=fopen((char *)filename, "r+");
      if (*fd == NULL)
	{
	  dLog(DLOG_MAJOR, "Problem opening <%s> with read.",filename);
	  perror("Error in Open");
	  fd = NULL;
	  return;
	}      
   }
}

void openAndMaybeRotate(const char *filename, FILE **fd, int threshold)
{
  // Will open a stream and if a threshold is reached it will rotate the file. 
   char timeStampFileName[DIR_PATH_MAX];
   struct stat file_status;

   //Check the size of the file. and rename if necessary.
   if (stat((char *)filename, &file_status) != 0)
   {
         dLog(DLOG_MAJOR, "Problem with stat call <%s>",filename);
         perror("Error in Stat");
         *fd = NULL;
         return;
   }
   dLog(DLOG_DEBUG_8, "File size for %s is %d", filename, file_status.st_size);
   if (file_status.st_size >= threshold)
   {
       sprintf(timeStampFileName, "%s.%s", filename, CURRENT_DATE_TIME_LOG);
               
       if (rename((char *)filename, (char *)timeStampFileName)!=0)
       {
          dLog(DLOG_MAJOR, "Problem with rename() call on <%s> to <%s>",
                    filename, timeStampFileName);
          perror("Error in Rename");
          *fd = NULL;
          return;
       }
       else
       {
          dLog(DLOG_STDLOG,"Rotating log <%s> to <%s>.",filename,timeStampFileName);
       }
    }
    *fd=fopen((char *)filename, "a+");
    if (*fd == NULL)
    {
         dLog(DLOG_MAJOR, "Problem opening <%s> for append.",filename);
         perror("Error in Open");
         // later, we MUST check for null before writing to this fd //
         fd = NULL;
         return;
    }
    dLog(DLOG_DEBUG_8, "opened %s for append", filename);
}
