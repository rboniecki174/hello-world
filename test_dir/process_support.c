/************************************************************************************/
/*                                                                                  */
/* process_support.c  RUN-FILE support for processes                      R00-00.01 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains the routines that are used to support the OS/32 sub-system    */
/* and the RUN-FILE assignment mechanism.                                           */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   Various based upon the arguments of the functions.                             */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* The "C" version of each function/subroutine is first and the FORTRAN interface   */
/* function/subroutine is second.  The FORTRAN interface function/subroutine        */
/* processes the arguments into a "C" function call.                                */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        11/Mar/1999  New file created from process_asn.f         */
/* R00-00.01  DRLJr        24/May/1999  Added routine Set_SYSEXC()                  */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Include Files                                                                    */
/*==================================================================================*/

#include <stdio.h>
#include <string.h>

#include "private_data.h"

/*==================================================================================*/
/* Static Data Definitions                                                          */
/*==================================================================================*/

static char fmt_lu_range[] =
   { "process_support.c (%-16s) : LU %3d is out of range %3d - %3d\n" };

int      GetFD_(int      * lu);

/*==================================================================================*/
/*                                                                                  */
/* Check_If_TCP()                                                                   */
/*                                                                                  */
/* This function is used to detirmine if the specified logical unit is attached to  */
/* a TCP/IP connection and to return the Unix File Descriptor.                      */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   IsTCP   - This is a returned flag that is true (1) to indicate that Lu is      */
/*           - attached to a TCP/IP connection or false (0) to indicate that it is  */
/*           - not attached to a TCP/IP connection.                                 */
/*   UnixFd  - This is the UNIX File Descriptor that is being used for the logical  */
/*           - unit specified by Lu.                                                */
/*                                                                                  */
/*==================================================================================*/

void Check_If_TCP(int        Lu, int      * YesNo, int      * UnixFd)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,"Check_If_TCP()",Lu,0L,MAXIMUM_LU);
    (*YesNo)   =  FALSE;
    (*UnixFd)  =  -1L;
  }
  else {
    (*YesNo)   =  private_data.lu_is_tcp[Lu];
    (*UnixFd)  =  private_data.lu_to_fd[Lu];
  }
  return;
}

void check_if_tcp_(int      * Lu, int      * YesNo, int      * UnixFd)
{ 
  Check_If_TCP((*Lu),YesNo,UnixFd);
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Check_If_SYSEXC()                                                                */
/*                                                                                  */
/* This function is used to detirmine if the calling process is identified as a     */
/* SYSEXC program/process.                                                          */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   YesNo   - This is a returned flag that is true (1) to indicate that the        */
/*           - process is a SYSEXC or false (0) to indicate that it is not a SYSEXC */
/*           - process.                                                             */
/*                                                                                  */
/*==================================================================================*/

void Check_If_SYSEXC(int      * YesNo)
{
  (*YesNo)   =  private_data.is_sysexc;
  return;
}

void check_if_sysexc_(int      * YesNo)
{ 
  Check_If_SYSEXC(YesNo);
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Set_SYSEXC()                                                                     */
/*                                                                                  */
/*==================================================================================*/

void Set_SYSEXC(int      yesno)
{
  private_data.is_sysexc = yesno;
  return;
}

void set_sysexc_(int      * yesno)
{
  Set_SYSEXC((*yesno));
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Set_Lu_FD()                                                                      */
/*                                                                                  */
/* This function is used to set the TCP/IP flag and set the UNIX Fd associated with */
/* a Logical Unit.                                                                  */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   IsTCP   - This is a returned flag that is true (1) to indicate that Lu is      */
/*           - attached to a TCP/IP connection or false (0) to indicate that it is  */
/*           - not attached to a TCP/IP connection.                                 */
/*   UnixFd  - This is the UNIX File Descriptor that is being used for the logical  */
/*           - unit specified by Lu.   If a negative 1 (-1L) is passed then this    */
/*           - argument will be updated with the appropriate value                  */
/*                                                                                  */
/*==================================================================================*/

void Set_Lu_FD(int        Lu, int        YesNo, int        UnixFd)
{
  int          UnixFDValue;

  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Set_Lu_FD()",0L,MAXIMUM_LU);
  }
  else {
    if(UnixFd < 0L) {
      UnixFDValue = (int     ) GetFD_(&Lu);         /* Use FORTRAN RTL to get value */
    }
    else {
      UnixFDValue = UnixFd;
    }
    private_data.lu_to_fd[Lu]    = UnixFDValue;
    private_data.lu_is_tcp[Lu]   = YesNo;
 }

  return;
}

void set_lu_fd_(int      * Lu, int      * YesNo, int      * UnixFd)
{ 
  Set_Lu_FD((*Lu),(*YesNo),(*UnixFd));
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Get_Lu_FD()                                                                      */
/*                                                                                  */
/* This function is used to get the TCP/IP flag and get the UNIX Fd associated with */
/* a Logical Unit.                                                                  */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   IsTCP   - This is a returned flag that is true (1) to indicate that Lu is      */
/*           - attached to a TCP/IP connection or false (0) to indicate that it is  */
/*           - not attached to a TCP/IP connection.                                 */
/*   UnixFd  - This is the UNIX File Descriptor that is being used for the logical  */
/*           - unit specified by Lu.   If a negative 1 (-1L) is passed then this    */
/*           - argument will be updated with the appropriate value                  */
/*                                                                                  */
/*==================================================================================*/

void Get_Lu_FD(int        Lu, int      * YesNo, int      * UnixFd)
{

  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_FD()",0L,MAXIMUM_LU);
    (*UnixFd) =   -1L;
    (*YesNo)  = FALSE;
  }
  else {
    (*UnixFd) = private_data.lu_to_fd[Lu];
    (*YesNo)  = private_data.lu_is_tcp[Lu];
 }

  return;
}

void get_lu_fd_(int      * Lu, int      * YesNo, int      * UnixFd)
{ 
  Get_Lu_FD((*Lu),YesNo,UnixFd);
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Clr_Lu_FD()                                                                      */
/*                                                                                  */
/* This function is used to clear the TCP/IP flag and the UNIX Fd associated with   */
/* a Logical Unit.                                                                  */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be cleared                       */
/*                                                                                  */
/*==================================================================================*/

void Clr_Lu_FD(int        Lu)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Clr_Lu_FD()",0L,MAXIMUM_LU);
  }
  else {
    private_data.lu_to_fd[Lu]    = -1L;
    private_data.lu_is_tcp[Lu]   = FALSE;
 }

  return;
}

void clr_lu_fd_(int      * Lu)
{ 
  Clr_Lu_FD((*Lu));
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Set_Lu_DCOD()                                                                    */
/*                                                                                  */
/* This function is used to set the Device Code for the specified logical unit.     */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Dcode   - This is the device code to be used by the OS/32 I/O subsystem.       */
/*           - The device code controls how the OS/32 I/O subsystem performs I/O to */
/*           - the device or file attached to the Lu.                               */
/*                                                                                  */
/*==================================================================================*/

void Set_Lu_DCOD(int        Lu, int        Dcode)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Set_Lu_DCOD()",0L,MAXIMUM_LU);
  }
  else {
    private_data.lu_dcod[Lu]    = Dcode;
 }

  return;
}

void set_lu_dcod_(int      * Lu, int      * Dcode)
{ 
  Set_Lu_DCOD((*Lu),(*Dcode));
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Get_Lu_DCOD()                                                                    */
/*                                                                                  */
/* This function is used to get the Device Code for the specified logical unit.     */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Dcode   - This is the device code to be used by the OS/32 I/O subsystem.       */
/*           - The device code controls how the OS/32 I/O subsystem performs I/O to */
/*           - the device or file attached to the Lu.                               */
/*                                                                                  */
/*==================================================================================*/

void Get_Lu_DCOD(int        Lu, int      * Dcode)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_DCOD()",0L,MAXIMUM_LU);
  }
  else {
    (*Dcode) = private_data.lu_dcod[Lu];
 }

  return;
}

void get_lu_dcod_(int      * Lu, int      * Dcode)
{ 
  Get_Lu_DCOD((*Lu),Dcode);
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Set_Lu_RECL()                                                                    */
/*                                                                                  */
/* This function is used to set the Record Length value that is associated with the */
/* specified Logical Unit.                                                          */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Recl    - This is the record length that is to be used by the I/O subsystem.   */
/*                                                                                  */
/*==================================================================================*/

void Set_Lu_RECL(int        Lu, int        Recl)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Set_Lu_RECL()",0L,MAXIMUM_LU);
  }
  else {
    private_data.lu_recl[Lu]    = Recl;
 }

  return;
}

void set_lu_recl_(int      * Lu, int      * Recl)
{ 
  Set_Lu_RECL((*Lu),(*Recl));
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Get_Lu_RECL()                                                                    */
/*                                                                                  */
/* This function is used to get the Record Length value that is associated with the */
/* specified Logical Unit.                                                          */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Recl    - This is the record length that is to be used by the I/O subsystem.   */
/*                                                                                  */
/*==================================================================================*/

void Get_Lu_RECL(int        Lu, int      * Recl)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_RECL()",0L,MAXIMUM_LU);
  }
  else {
    (*Recl) = private_data.lu_recl[Lu];
 }

  return;
}

void get_lu_recl_(int      * Lu, int      * Recl)
{ 
  Get_Lu_RECL((*Lu),Recl);
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Set_Lu_SIZE()                                                                    */
/*                                                                                  */
/* This function is used to set the number of records that are in the file or       */
/* device that is associated with a Logical Unit.                                   */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Size    - This is the number of records that are currently in the file.  This  */
/*           - value is updated as appropriate.                                     */
/*                                                                                  */
/*==================================================================================*/

void Set_Lu_SIZE(int        Lu, int        Size)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Set_Lu_SIZE()",0L,MAXIMUM_LU);
  }
  else {
    private_data.lu_size[Lu]    = Size;
 }

  return;
}

void set_lu_size_(int      * Lu, int      * Size)
{ 
  Set_Lu_SIZE((*Lu),(*Size));
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Get_Lu_SIZE()                                                                    */
/*                                                                                  */
/* This function is used to get the number of records that are in the file or       */
/* device that is associated with a Logical Unit.                                   */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Size    - This is the number of records that are currently in the file.  This  */
/*           - value is updated as appropriate.                                     */
/*                                                                                  */
/*==================================================================================*/

void Get_Lu_SIZE(int        Lu, int      * Size)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_SIZE()",0L,MAXIMUM_LU);
  }
  else {
    (*Size) = private_data.lu_size[Lu];
 }

  return;
}

void get_lu_size_(int      * Lu, int      * Size)
{ 
  Get_Lu_SIZE((*Lu),Size);
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Set_Mirr_FD()                                                                    */
/*                                                                                  */
/* This function is used to set the Unix File Descriptor for use with the mirror    */
/* file when the mirror command and mirror options is specified.                    */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   UnixFd  - This is the Unix File Descriptor used with the mirror file.          */
/*                                                                                  */
/*==================================================================================*/

void Set_Mirr_FD(int        Lu, int        UnixFd)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Set_Mirr_FD()",0L,MAXIMUM_LU);
  }
  else {
    private_data.lu_mirror[Lu]    = UnixFd;
 }

  return;
}

void set_mirr_fd_(int      * Lu, int      * UnixFd)
{ 
  Set_Mirr_FD((*Lu),(*UnixFd));
  return;
}

/*==================================================================================*/
/*                                                                                  */
/* Get_Mirr_FD()                                                                    */
/*                                                                                  */
/* This function is used to get the Unix File Descriptor for use with the mirror    */
/* file when the mirror command and mirror options is specified.                    */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   UnixFd  - This is the Unix File Descriptor used with the mirror file.          */
/*                                                                                  */
/*==================================================================================*/

void Get_Mirr_FD(int        Lu, int      * UnixFd)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Mirr_FD()",0L,MAXIMUM_LU);
  }
  else {
    (*UnixFd) = private_data.lu_mirror[Lu];
 }

  return;
}

void get_mirr_fd_(int      * Lu, int      * UnixFd)
{ 
  Get_Mirr_FD((*Lu),UnixFd);
  return;
}


/*==================================================================================*/
/*                                                                                  */
/* Get_Lu_Filename()                                                                */
/*                                                                                  */
/* This function is used to get the Unix File Descriptor for use with the mirror    */
/* file when the mirror command and mirror options is specified.                    */
/*                                                                                  */
/* The arguments to the function are                                                */
/*                                                                                  */
/*   Lu      - This is the logical unit that is to be checked                       */
/*   Filename- This is the Unix File Name that is assigned to the logical unit.     */
/*                                                                                  */
/*==================================================================================*/

void Get_Lu_Filename(int        Lu, unsigned char * UnixFileName)
{
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_Filename()",0L,MAXIMUM_LU);
  }
  else {
    strcpy((char *)UnixFileName,(char *)private_data.lu_filename[Lu]);
  }

  return;
}

void get_lu_filename_(int      * AdrLu, unsigned char * UnixFileName, int      namelen)
{ 
  int           maxlen;
  int           Lu;

  Lu   = (*AdrLu);
  if((Lu < 0L) || (Lu > MAXIMUM_LU)) {
    fprintf(stderr,fmt_lu_range,Lu,"Get_Lu_Filename()",0L,MAXIMUM_LU);
  }
  else {
    memset(UnixFileName,(int     ) ' ', namelen);
    maxlen     =  strlen((char *)private_data.lu_filename[Lu]);
    if(maxlen > (namelen - 1L)) {
      maxlen = namelen - 1L;
    }
    memcpy(UnixFileName,private_data.lu_filename[Lu],maxlen);
  }

  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
