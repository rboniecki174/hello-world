/************************************************************************************/
/*                                                                                  */
/* process_runfiles.c  RUN-FILE support for processes                     R00-00.00 */
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
/* R00-00.00  DRLJr        29/Mar/1999  New file created for process_asn support    */
/*                                                                                  */
/************************************************************************************/


/*==================================================================================*/
/* Static Data Definitions                                                          */
/*==================================================================================*/

static unsigned char       runfile_env[]    = { "RUN_FILES"    };
static unsigned char       runfile_path[]   = { "./RUN-FILES/" };
static unsigned char     * runfile_ext[]    = {
                                                (unsigned char *)".run",
                                                (unsigned char *)".Run",
                                                (unsigned char *)".RUN",
                                               (UCHAR *) NULL };


/*==================================================================================*/
/* MakeASNFileName()                                                                */
/*==================================================================================*/

static void MakeASNFileName(UCHAR * BaseName, UCHAR * Extent, UCHAR * Buffer)
{
  UCHAR        * runpath;                    /* Address of the Run Path String      */
  int            length;                     /* Working Length Variable             */

  runpath = (UCHAR *) getenv((char *)runfile_env);   /* Get a potential override value      */
  if(runpath == (UCHAR *) NULL) {            /* No override value is present        */
    runpath = &(runfile_path[0]);            /* Use the standard value              */
  }                                          /*                                     */

  strcpy((char *)Buffer,(char *)runpath);                    /* Insert the base run-path string     */
  length = strlen((char *)Buffer);                   /* Find the length of the string       */
  if(Buffer[length-1] != '/') {              /* Need a directory character (/)      */
    strcat((char *)Buffer,"/");                      /* Add the character                   */
  }                                          /*                                     */
  strcat((char *)Buffer,(char *)BaseName);                   /* Now add the base name               */
  strcat((char *)Buffer,(char *)Extent);                     /* Add the extent                      */

  return;                                    /* Return to the caller                */
}                                            /*                                     */

//static void makeasnfilename_(UCHAR    * BaseName,   UCHAR    * Extent,
//                             UCHAR    * Buffer,
//                             int        BaseNameLn, int        ExtentLn,
//                             int        BufferLn)
//{
//  int          index;                        /* Last Non-Blank in BaseName          */
//  int          jndex;                        /* Last Non-Blank in Extent            */
//  int          kndex;                        /* Length of the WrkBuffer text        */
//  int          length;                       /* Working Length                      */
//  UCHAR        WrkBaseName[WRK_NAMESIZE];    /* Working BaseName                    */
//  UCHAR        WrkExtent[WRK_NAMESIZE];      /* Working Extent                      */
//  UCHAR        WrkBuffer[WRK_NAMESIZE];      /* Working Buffer                      */
//
//  index        = lnblnk(BaseName,BaseNameLn);/* Locate last non-blank               */
//  jndex        = lnblnk(Extent,  ExtentLn);  /* Locate last non-blank               */
//
//  memset(WrkBaseName,0x00,WRK_NAMESIZE);     /* Initialize the work space           */
//  memset(WrkExtent,0x00,WRK_NAMESIZE);       /* Initialize the work space           */
//  memset(WrkBuffer,0x00,WRK_NAMESIZE);       /* Initialize the work space           */
//
//  length  = ((index < WRK_NAMESIZE-2L) ? index : (WRK_NAMESIZE-2L));/*Select min len*/
//  memcpy(WrkBaseName,BaseName,length);                  /* Move to work space       */
//
//  length  = ((jndex < WRK_NAMESIZE-2L) ? jndex : (WRK_NAMESIZE-2L));/*Select min len*/
//  memcpy(WrkExtent  ,Extent  ,length);                  /* Move to work space       */
//
//  MakeASNFileName(WrkBaseName, WrkExtent, WrkBuffer);   /* Build the filename       */
//
//  kndex   = strlen((char *)WrkBuffer);                          /* Length of actual name    */
//  length  = ((kndex < BufferLn) ? kndex : BufferLn);    /* Use minimum length       */
//  memset(Buffer,(int     ) ' ',BufferLn);               /* Clear to spaces          */
//  memcpy(Buffer,WrkBuffer,length);                      /* Move file path & name    */
//
//  return;                                               /* Return to the caller     */
//}                                                       /*                          */


/*==================================================================================*/
/* OpenASNFile()                                                                    */
/*==================================================================================*/

static int      OpenASNFile(FILE ** stream, UCHAR * File)
{
  int             status;                    /* Working status variable             */
  FILE          * wstream;                   /* Working Stream for File             */

  status       =  access((char *)File,F_OK);         /* Does the file exist                 */
  if(status == 0L) {                         /* The file exists                     */
    status     =  access((char *)File,R_OK);         /* Can the file be read ?              */
    if(status == 0L) {                       /* The file can be read                */
      wstream  =  fopen((char *)File,(char *)"r");           /* Open the file                       */
      if(wstream != (FILE *) NULL) {         /* The file is open                    */
        (*stream) = wstream;                 /* Return the stream pointer           */
        status    = 0L;                      /* Indicate good status                */
      }                                      /*                                     */
      else {                                 /* The file could not be open'ed       */
        status   =  -3L;                     /* Indicate file can not be read       */
      }                                      /*                                     */
    }                                        /*                                     */
    else {                                   /* File Can not be read                */
      status   =  -2L;                       /* Indicate file can not be read       */
    }                                        /*                                     */
  }                                          /*                                     */
  else {                                     /* File Does not exist                 */
    status     =  -1L;                       /* Indicate does not exist             */
  }                                          /*                                     */

  return(status);                            /* Return to the caller                */
}

//static int      openasnfile_(FILE ** stream, UCHAR * File, int      length)
//{
//  int             status;                    /* Working status variable             */
//  int             wlength;                   /* Length of bytes to move             */
//  UCHAR           WrkFile[WRK_NAMESIZE];     /* Working Filename                    */
//  
//  status       =  lnblnk(File,length);       /* Locate the last non-blank           */
//  memset(WrkFile,0x00,WRK_NAMESIZE);         /* Clear the work buffer               */
//  wlength  = ((length < WRK_NAMESIZE-2L) ? length : WRK_NAMESIZE-2L); /* Use min len*/
//  memcpy(WrkFile,File,wlength);              /* Move the filename for open call     */
// 
//  status   = OpenASNFile(stream,WrkFile);    /* Try to open the file                */
//
//  return(status);                            /* Return the status to the caller     */
//}                                            /*                                     */


/*==================================================================================*/
/* ReadASNFile()                                                                    */
/*==================================================================================*/

static int      ReadASNFile(FILE * stream, UCHAR * Buffer, int      length)
{
  int        status;                              /* Status of the I/O              */
  int        rdlen;                               /* Amount of data in buffer       */
  UCHAR    * cstatus;                             /* Character status               */
  UCHAR      wrkbuf[WRK_NAMESIZE];                /* Read Work Buffer               */

  cstatus   = (UCHAR *)fgets((char *)Buffer,length,stream);        /* Read a record of data          */
  if(cstatus == (UCHAR *) NULL) {                 /* EOF or I/O Error               */
    status = feof(stream);                        /* EOF status ?                   */
    if(status) {                                  /* EOF - return EOF status        */
      status = -1L;                               /* Set EOF status                 */
    }                                             /*                                */
    else {                                        /* I/O Error status               */
      status = -2L;                               /* Set I/O Error status           */
    }                                             /*                                */
  }                                               /*                                */
  else {                                          /* Good status                    */
    rdlen  = strlen((char *)Buffer);                      /* Amount of data read            */
    if(Buffer[rdlen-1L] == '\n') {                /* All the data was read          */
      Buffer[rdlen-1L] = '\0';                    /* Convert to a NULL              */
      status           = 0L;                      /* Indicate good read             */
    }                                             /*                                */
    else {                                        /* Get rest of the record         */
      for(;;) {                                   /* Locate the newline             */
        cstatus = (UCHAR *)fgets((char *)wrkbuf,WRK_NAMESIZE,stream); /* Read a block of data        */
        if(cstatus == (UCHAR *) NULL) {           /* EOF or I/O Error               */
          break;                                  /* Exit Processing Loop           */
        }                                         /*                                */
        rdlen  = strlen((char *)Buffer);                  /* Amount of data read            */
        if(Buffer[rdlen-1L] == '\n') {            /* All the data was read          */
          break;                                  /* End of the record was found    */
        }                                         /*                                */
      }                                           /*                                */
      status   = 1L;                              /* Indicate not all of record read*/
    }                                             /*                                */
  }                                               /*                                */
  return(status);                                 /* Return to the caller           */
}                                                 /*                                */

//static int      readasnfile_(FILE ** stream, UCHAR * Buffer, int      length)
//{
//  int        status;                              /* Status of the I/O              */
//  int        rdlen;                               /* Amount of data in buffer       */
//  int        blanklen;                            /* Amount of space to blank       */
//
//  status   = ReadASNFile((*stream),Buffer,length);/* Go and Read from the file      */
//  if(status >= 0L) {                              /* A Good Read ?                  */
//    rdlen  = strlen((char *)Buffer);                      /* Determine amount of data       */
//    blanklen = length - rdlen;                    /* Amount of space to blank       */
//    memset(Buffer, (int     ) ' ', blanklen);     /* Blank the buffer (FORTRAN)     */
//  }                                               /*                                */
//  return(status);                                 /* Return to the caller           */
//}                                                 /*                                */
  

/*==================================================================================*/
/* CloseASNFile()                                                                   */
/*==================================================================================*/

static int      CloseASNFile(FILE * stream)
{
  int         status;

  status    = fclose(stream);
  return(status);
}

//static int      closeasnfile_(FILE ** stream)
//{
//  int         status;
//
//  status    = CloseASNFile((*stream));
//  return(status);
//}


/*==================================================================================*/
/* AssignRunFile()                                                                  */
/*==================================================================================*/

static int      AssignRunFile(UCHAR * BaseName, FILE ** stream)
{
  int         status;                          /* Working Status                    */
  int         rtnstatus;                       /* Return status                     */
  int         index;                           /* Working index for the extensions  */
  UCHAR       BaseNameWrk[WRK_NAMESIZE];       /* Basename work space               */

  rtnstatus = -1L;                                             /* Indicate not found*/
  for(index=0;runfile_ext[index]!=(UCHAR *) NULL;index++) {    /* Loop thru extents */
    MakeASNFileName(BaseName,runfile_ext[index],BaseNameWrk);  /* Create the name   */
    status = OpenASNFile(stream,BaseNameWrk);                  /* Open the file     */
    if(status == 0L) {                                         /* File is open      */
      strcpy((char *)processasn.CALL_FileName[processasn.RunDepth],(char *)BaseNameWrk);
      rtnstatus = 0L;                                          /* Say all is okay   */
      break;                                                   /* Exit work loop    */
    }                                                          /*                   */
  }                                                            /*                   */
  return(rtnstatus);                                           /* Return to caller  */
}                                                              /*                   */

//static int      assignrunfile_(UCHAR * BaseName, FILE ** stream, int      BaseNameLn)
//{
//  int         length;                          /* Length of the work space          */
//  int         status;                          /* Return status                     */
//  UCHAR       BaseNameWrk[WRK_NAMESIZE];       /* Basename work space               */
//
//  length  = lnblnk(BaseName,BaseNameLn);       /* Find the last non-blank           */
//  length  = ((length < WRK_NAMESIZE-2L) ? length : WRK_NAMESIZE-2L); /* Use Min len */
//
//  memset(BaseNameWrk,0x00,WRK_NAMESIZE);       /* Clear the work space              */
//  memcpy(BaseNameWrk,BaseName,length);         /* Move the base name                */
//
//  status  = AssignRunFile(BaseNameWrk,stream); /* Call the "C" version              */
//
//  return(status);                              /* Return the status                 */
//}                                              /*                                   */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
