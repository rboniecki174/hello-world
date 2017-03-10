//************************************************************************************/
//*                                                                                  */
//* process_ftnclose.c  RUN-FILE support for FORTRAN Closes                R00-00.00 */
//*                                                                                  */
//* ================================================================================ */
//*                                                                                  */
//* Purpose:                                                                         */
//*                                                                                  */
//* This file contains the routine that is used  to perform FORTRAN CLOSEs.          */
//*                                                                                  */
//* This routine is responsible for closing the file or device for FORTRAN only.     */
//*                                                                                  */
//* ================================================================================ */
//*                                                                                  */
//* Arguments:                                                                       */
//*                                                                                  */
//*                                                                                  */
//* ================================================================================ */
//*                                                                                  */
//* Programming Notes:                                                               */
//*                                                                                  */
//* ================================================================================ */
//*                                                                                  */
//* Revision:                                                                        */
//*                                                                                  */
//* Revision   Who          When         Why                                         */
//* =========  ===========  ===========  =========================================== */
//* R00-00.00  DRLJr        01/Apr/1999  New file created for process_asn support    */
//* tt78919    jeffs        03-Feb-2014  Translate to C and simplify based on observation
//*                                                                                  */
//************************************************************************************/
//
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <private_data.h>

int open_lu_to_fd[MAXIMUM_LU];

void process_ftnclose_(int *Lu, int *RtnStatus)
{
      int fd;
      //check range
      if((*Lu < 0) || (*Lu >= MAXIMUM_LU)) {
         fprintf(stderr,"process_ftnclose_(): Error in range.  LU %d\n",*Lu);
         *RtnStatus = 1;   //error 
      }
         
      fd = fnum_store[*Lu];
      *RtnStatus=close(fd);
      if (fd == -1) {
         fprintf(stderr,"process_ftnclose_(): Error closing LU %d, errno=%d. <%s>\n",*Lu, errno, strerror(errno));
         *RtnStatus = 1;   //error 
      } 
      else {
        if (*Lu == 5)
            fprintf(stderr,"process_ftnclose_(): Note: LU 5 has been closed\n");
        else if (*Lu == 6)
            fprintf(stderr,"process_ftnclose_(): Note: LU 6 has been closed\n");
   
//      unload the local globals
        fnum_store[*Lu] = -1;
        recl_store[*Lu] = -1; 
     }
}
