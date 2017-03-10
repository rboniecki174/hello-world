//***********************************************************************************/
//                                                                               */
// process_ftnopen.c   RUN-FILE emulation for FORTRAN Opens                 R00-00.00 */
//                                                                               */
// ================================================================================ */
//                                                                               */
// Purpose:                                                                         */
//                                                                               */
// This file contains the routine that is used  to emulate FORTRAN OPENs.           */
//                                                                               */
// This routine is responsible for assigning the file or device for FORTRAN only.   */
//                                                                               */
// ================================================================================ */
//                                                                               */
// Arguments:                                                                       */
//                                                                               */
//                                                                               */
// ================================================================================ */
//                                                                               */
// Programming Notes:                                                               */
//                                                                               */
// ================================================================================ */
//                                                                               */
// Revision:                                                                        */
//                                                                               */
// Revision   Who          When         Why                                         */
// =========  ===========  ==-========  =========================================== */
// R00-00.00  DRLJr        01/Apr-1999  New file created for process_asn support    */
// R01-00.00  jeffsun      14-Mar-2001  Remove Recl for Sun f77 to avoid warning if */
//                                   SEQUENTIAL access only                      */
// tt62010    GJ           27-May-2011  Support APPEND mode for file open
// tt78919    jeffs        03-Feb-2014  Translate to C and simplify based on observation
//                                      of the flags used in fortran.
//***********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <private_data.h>


#define OLD 0
#define NEW 1
#define RENEW 2
#define SEQUENTIAL 0
#define DIRECT 1


int process_copen( int lu, char* filename, int access, int oldnew, int recl)
{
   int retstat = 0;
   int fd;
   int flags =  O_RDWR | 0100000;

   if((lu <  0) || (lu >= MAXIMUM_LU)) {
      fprintf(stderr,"process_copen(): Error in range.  LU is %d\n",lu);
      retstat = 1;   //error 
   }
   
   if (oldnew == NEW) {     
     flags |= O_CREAT;      //default is to create (NEW) unless marked old.
   }
   else if ((oldnew==RENEW)) {
     flags |= O_TRUNC;
   }

 //------------------------------------------
 // Note for 0_CREAT the file permisions are provided in the last argument, mode.  
 // This argument is ignored otherwise.
 //------------------------------------------
   fd = open(filename, flags, 0666);
 //------------------------------------------
   fprintf(stderr,"process_copen(): Opening %s with flags=%X returns %d\n", filename, flags, fd);

   if (fd == -1) {
       fprintf(stderr,"process_copen(): Error opening LU %d, errno=%d. <%s>\n", lu, errno, strerror(errno));
       retstat = 1;   //error
   }
   else {
      //fprintf(stderr,"process_copen(): File has been opened. Storing fd=%d for lu %d\n",
      //  fd, lu);
      fnum_store[lu] = fd; 
      if ((access == DIRECT) && (recl > 0)) {
        // store reclen if direct is specified and reclen > 0
        recl_store[lu] = recl; 
      } 
   }

   return retstat;
}

//***********************************************************************************/
//                                                                               */
//***********************************************************************************/

void process_ftnopen_(int* lu, 
                      char *file, 
                      char *ftn_access, 
                      char *ftn_status,   
                      char *ftn_form, 
                      int* ftn_recl,
                      int* ftn_readonly, 
                      int* rtnstatus)
{
   //----------------------------------------------
   //..  Argument Declarations
   // Integer*4          Lu                   // Logical that is to be assigned
   // Character*[*]      File                 // Name of file being assigned
   // Character*[*]      FTN_Access           // FORTRAN Access
   // Character*[*]      FTN_Status           // FORTRAN Status
   // Character*[*]      FTN_Form             // FORTRAN Form
   // Integer*4          FTN_Recl             // FORTRAN Record Length
   // Integer*4          FTN_ReadOnly         // FORTRAN Read Only Flag
   // Integer*4          RtnStatus            // Return Status for caller
   //----------------------------------------------

   int oldnew = NEW;  //default
   int access = SEQUENTIAL; //default

   if (strncmp(ftn_status,"OLD",3)==0) {
      oldnew = 0;                    //default is new = 1
   }  
   else if (strncmp(ftn_status,"RENEW",3)==0) {
      oldnew = RENEW;
   } 
   if (strncmp(ftn_access,"DIRECT",6)==0) {
      access = DIRECT;
   }

   *rtnstatus = process_copen(*lu, file, access, oldnew, *ftn_recl);

}
