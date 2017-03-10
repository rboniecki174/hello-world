/************************************************************************************/
/*                                                                                  */
/* process_assign.c    RUN-FILE support (Assignment Code)                 R00-00.02 */
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
/* THIS FILE REQUIRES process_base.c                                                */
/*                                                                                  */
/* goto's are used where their use would allow the code to simplified to eliminate  */
/* the deep nesting of if statements.                                               */
/*                                                                                  */
/* For in-line comments to be recognized there must be at least 1 blank or tab      */
/* preceeding the comment characters (# or *).                                      */
/*                                                                                  */
/* The "C" version of each function/subroutine is first and the FORTRAN interface   */
/* function/subroutine is second.  The FORTRAN interface function/subroutine        */
/* processes the arguments into a "C" function call.                                */
/*                                                                                  */
/* MACROs are used to hide the various definitions of the common block from the     */
/* program.  The variables in the common block processasn have fixed formats for    */
/* names.  The names for non-FORTRAN related variables are:                         */
/*                                                                                  */
/*        Value_<name>         Flag_<name>                                          */
/*                                                                                  */
/* The names for the FORTRAN related variables are:                                 */
/*                                                                                  */
/*        Flag_FTN_<name>      FTN_OPEN_<name>                                      */
/*                                                                                  */
/* Other variables in the common block are not masked.                              */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        13/Apr/1999  process_common.c split into two files       */
/* R00-00.01  DRLJr        19/May/1999  File Prefill routine was being called when  */
/*                                      it should not have been called.  Added if() */
/*                                      logic testing Flag_FTN_RENEW to fix.        */
/* R00-00.02  DRLJr        21/Jul/1999  Changed %29.29s to %-29.29s for the Opened  */
/*                                      TCP port message.                           */
// tt62010    GJ           27/May/2011  Support APPEND mode for file open
// tt78918    jeffs        21-Apr-2014  Fix a bug to add conditional to call
//                                      of dskfilesize to eliminate error msg.
// tt80093    gj           22/May/2014  Clean up prototypes
/*                                                                                  */
/************************************************************************************/
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

int lstat(const char *path, struct stat *buf);
void get_hostport_(unsigned char * input,   unsigned char * host,
                           unsigned char * port,    int           * status,
                           int             length,  int            hlength,
                           int             slength);

int get_hostport(unsigned char *input, unsigned char *host,unsigned char *port);

void open_tcp_as_server_(int      * tcpfd,
   char     * inhost, 
   char     * inservice,
   int        len_inhost,
   int        len_inservice );

void open_tcp_as_client_(int      * tcpfd,
   char     * inhost, 
   char     * inservice,
   int        len_inhost,
   int        len_inservice );

void Set_Lu_SIZE(int        Lu, int        Size);
void Set_Lu_RECL(int        Lu, int        Recl);
void Set_Lu_DCOD(int        Lu, int        Dcode);
void Set_Lu_FD(int        Lu, int        YesNo, int        UnixFd);
void dskfilesize(int      lu, int      dcode, int      recl, int      * nrecs);
int ttyb_init_(int      * lu,int      * flg);
int ttys_init_(int      * lu,int      * flg);
int ttym_init_(int      * lu,int      * flg);
int      mirrorlu_(unsigned char * mirrpath, unsigned char * filepath,
                   int             mirrlen,  int             filelen);


/*==================================================================================*/
/* Call_RunFile()                                                                   */
/*==================================================================================*/

static int      Call_RunFile(UCHAR * rdbuffer, int      * offset, int       length, 
                             int       cmd)
{
  int       status;                         /* Working status                     */
  int       saveoffset;                     /* Saved offset value                 */
  UCHAR   * adrbuf;                         /* Short hand for CALL_Filename       */
  UCHAR     BaseName[WRK_NAMESIZE];               /* Name to use                        */

  if((processasn.RunDepth + 1L) >= RUN_DEPTH) {
    adrbuf = processasn.CALL_FileName[processasn.RunDepth];
    DSPERROR((*offset));
    fprintf(stderr,fmt10260,Record[processasn.RunDepth],(*offset)+1L,adrbuf);
    status = -1L;
  }
  else {
    (*offset)  =  fnblnk(rdbuffer,(*offset),length);
    saveoffset = (*offset);
    status = Cmd_GetPath(rdbuffer,offset,length,cmd,BaseName,WRK_NAMESIZE-2L);
    if(status == 0L) {
      processasn.RunDepth++;
      status = AssignRunFile(BaseName, &Stream[processasn.RunDepth]);
      if(status < 0) {
        processasn.RunDepth--;
        (*offset) = saveoffset;
        adrbuf = processasn.CALL_FileName[processasn.RunDepth];
        DSPERROR((*offset));
        fprintf(stderr,fmt10010,BaseName);
        fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
        status = -1L;
      }
      else {
        status = 0L;
      }
    }
    else {
      fflush(stderr);
      DSPERROR((*offset));
      fprintf(stderr,fmt10225);
      fflush(stderr);
      adrbuf = processasn.CALL_FileName[processasn.RunDepth];
      fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
      status = -1L;
    }
  }
  return(status);
}


/*==================================================================================*/
/* Call_AsnFile()                                                                   */
/*==================================================================================*/

static int      Call_AsnFile(UCHAR * rdbuffer, int      * offset, int       length,
                             int       cmd)
{
  int       status;                         /* Working status                       */
  int       saveoffset;                     /* Saved offset value                   */
  int       index;                          /* Working Index                        */
  int       filelu;                         /* Logical Unit for the file            */
  int       valid;                          /* Validate the options                 */
  UCHAR   * adrbuf;                         /* Short hand for CALL_Filename         */
  UCHAR     BaseName[WRK_NAMESIZE];         /* Name to use                          */
  UCHAR     Option[WRK_NAMESIZE];           /* Option String                        */

  /*--------------------------------------------------------------------------------*/
  /* Initialize for processing                                                      */
  /*--------------------------------------------------------------------------------*/

  Process_ASN_Clear_Common();               /* Clear for ASSIGN processing          */

  adrbuf   = processasn.CALL_FileName[processasn.RunDepth];

  /*--------------------------------------------------------------------------------*/
  /* Position to the logical unit                                                   */
  /*--------------------------------------------------------------------------------*/

  (*offset)  =  fnblnk(rdbuffer,(*offset),length);   /* Move to non-blank character */
  saveoffset = (*offset);                            /* Save the offset             */
  index      = fblnk(rdbuffer,(*offset),length);     /* Locate end+1 of lu number   */
  if((*offset) >= length) {                          /* Missing LU                  */
    DSPERROR((*offset));                             /* Display error message       */
    fprintf(stderr,fmt10222,Record[processasn.RunDepth],(*offset)+1L,adrbuf);
    fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
    status   = -1L;                                  /* Set the status to errors    */
    goto return_to_caller;                           /* Go and exit routine         */
  }                                                  /*                             */
  
  /*--------------------------------------------------------------------------------*/
  /* Extract the logical unit value                                                 */
  /*--------------------------------------------------------------------------------*/

  filelu     = getvalue(rdbuffer,(*offset),index-1L);/* Decode and extract the lu   */
  if((filelu < 0) || (filelu >= MAXIMUM_LU)) {       /* Error or out of range       */
    DSPERROR((*offset));                             /* Display error message       */
    fprintf(stderr,fmt10025,Record[processasn.RunDepth],(*offset)+1L,adrbuf);
    status   = -1L;                                  /* Set the status to errors    */
    goto return_to_caller;                           /* Go and exit routine         */
  }                                                  /*                             */

  (*offset)  = index;                                /* Move after the lu number    */

  /*--------------------------------------------------------------------------------*/
  /* Get the filename into the working buffer                                       */
  /*--------------------------------------------------------------------------------*/

  (*offset)  =  fnblnk(rdbuffer,(*offset),length);   /* Move to the text            */
  saveoffset = (*offset);                            /* Save the offset             */
  status     = Cmd_GetPath(rdbuffer,offset,length,cmd,BaseName,WRK_NAMESIZE-2L);
  if(status != 0L) {                                 /* Problem with the text       */
    status   = -1L;                                  /* Set the status to errors    */
    goto return_to_caller;                           /* Go and exit routine         */
  }                                                  /*                             */


  /*--------------------------------------------------------------------------------*/
  /* Process any options that are present                                           */
  /*--------------------------------------------------------------------------------*/

  for(;;) {                                          /* Loop until we are done      */
    status    =  Check_EOL(rdbuffer,offset,length);  /* Move to option or EOL(CMNT) */
    if(status == 0L) {                               /* We have reached the end     */
      break;                                         /* Leave the processing loop   */
    }                                                /*                             */
    status = Get_ASNOption(rdbuffer,offset,length,Option);
    if(status < 0L) {
      DSPERROR((*offset));                           /* Display the error text      */
      if(status == -1L) {                            /*                             */
        fprintf(stderr,fmt10040);                    /*                             */
      }                                              /*                             */
      else if(status == -2L) {                       /*                             */
        fprintf(stderr,fmt10041);                    /*                             */
      }                                              /*                             */
      else if(status == -3L) {                       /*                             */
        fprintf(stderr,fmt10042);                    /*                             */
      }                                              /*                             */
      else {                                         /*                             */
        fprintf(stderr,fmt10040);                    /*                             */
      }                                              /*                             */
      fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
    }                                                /*                             */
  }                                                  /*                             */


  /*--------------------------------------------------------------------------------*/
  /* Validate the options that were located                                         */
  /*--------------------------------------------------------------------------------*/

  valid   = processasn.Flag_CO       + processasn.Flag_IN      + 
            processasn.Flag_BRKSHRT  + processasn.Flag_SYSEXC  + 
            processasn.Flag_MARPAD   + processasn.Flag_PR      +
            processasn.Flag_CON      + processasn.Flag_LOG;
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  valid   = processasn.Flag_CO       + processasn.Flag_IN      + 
            processasn.Flag_SERVER   + processasn.Flag_CLIENT  +
            processasn.Flag_PR       + processasn.Flag_LOG;
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  valid   = processasn.Flag_SERVER   + processasn.Flag_CLIENT  +
            processasn.Flag_LOCAL;
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  valid   = processasn.Flag_FTN_OLD     + processasn.Flag_FTN_NEW     +
            processasn.Flag_FTN_SCRATCH + processasn.Flag_FTN_UNKNOWN + 
            processasn.Flag_FTN_RENEW;
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  valid   = processasn.Flag_FTN_SEQUENTIAL + processasn.Flag_FTN_DIRECT +
            processasn.Flag_FTN_APPEND;   //tt62010
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  valid   = processasn.Flag_FTN_FORMATTED  + processasn.Flag_FTN_UNFORMATTED +
            processasn.Flag_FTN_PRINT;
  if(valid > 1L) {
    fprintf(stderr,fmt10050,Record[processasn.RunDepth]);
    status = -1L;
  }

  if(status != 0L) {
    goto return_to_caller;                           /* Go and exit routine         */
  }                                                  /*                             */


  /*--------------------------------------------------------------------------------*/
  /* Set the values for performing basic assignment operations                      */
  /*--------------------------------------------------------------------------------*/

  processasn.Value_Lu  = filelu;                    /* Set the Logical Unit number  */
/*memcpy(processasn.Value_Filename,BaseName,strlen(BaseName));*/ /* Move filename   */
  strcpy((char *)processasn.Value_Filename,(char *)BaseName);      /* Move filename                 */

  if((processasn.Flag_MARPAD) && (filelu == MDEQTRNS_LU1)) {
    processasn.Value_Lu2 = MDEQTRNS_LU2;
  }

  if((processasn.Flag_BRKSHRT) && (filelu == BRKSHRT_LU1)) {
    processasn.Value_Lu2 = BRKSHRT_LU2;
  }

  if(processasn.Flag_Tie) {
    processasn.Value_Lu2 = processasn.Value_Tie;
  }

  /*--------------------------------------------------------------------------------*/
  /* Report all as okay                                                             */
  /*--------------------------------------------------------------------------------*/

  status     =  0L;                                  /* Say all is okay             */
  goto return_to_caller;                             /* Go and exit routine         */

  /*--------------------------------------------------------------------------------*/
  /* Return to the caller                                                           */
  /*--------------------------------------------------------------------------------*/

  return_to_caller :                                 /* Exit point for the routine  */

  return(status);                                    /* Return to caller            */
}


/*==================================================================================*/
/* Set_Defaults()                                                                   */
/*==================================================================================*/

static void Set_Defaults(void)
{

  /*--------------------------------------------------------------------------------*/
  /* Set the underlaying record length based upon the various settings              */
  /*--------------------------------------------------------------------------------*/

  if((processasn.Flag_IN) >  0L) {
    processasn.Value_Recl = processasn.Value_IN;
  }
  else if((processasn.Flag_CO) >  0L) {
    processasn.Value_Recl = 256L;
  }
  else if((processasn.Flag_FTN_RECL) >= 0L) {
    processasn.Value_Recl = -1L;
  }
  else {
    processasn.Value_Recl = -1L;
  }


  /*--------------------------------------------------------------------------------*/
  /* Generate the appropriate defaults for settings                                 */
  /*--------------------------------------------------------------------------------*/

  if(processasn.Flag_CO) {                           /* Set defaults for CO files   */
    if(processasn.Flag_FTN_STATUS == 0L) {
      memcpy(processasn.FTN_OPEN_Status,"OLD",strlen("OLD"));
      processasn.Flag_FTN_STATUS  =  1L;
    }
    if(processasn.Flag_FTN_ACCESS == 0L) {
      memcpy(processasn.FTN_OPEN_Access,"DIRECT",strlen("DIRECT"));
      processasn.Flag_FTN_ACCESS  =  1L;
    }
    if(processasn.Flag_FTN_FORM == 0L) {
      memcpy(processasn.FTN_OPEN_Form,"UNFORMATTED",strlen("UNFORMATTED"));
      processasn.Flag_FTN_FORM  =  1L;
    }
    if(processasn.Flag_FTN_RECL == 0L) {
      processasn.FTN_OPEN_Recl  =  1L;
      processasn.Flag_FTN_RECL  =  1L;
    }
  }

  if(processasn.Flag_IN) {                            /* Set defaults for IN files   */
    if(processasn.Flag_FTN_STATUS == 0L) {
      memcpy(processasn.FTN_OPEN_Status,"OLD",strlen("OLD"));
      processasn.Flag_FTN_STATUS  =  1L;
    }
    if(processasn.Flag_FTN_ACCESS == 0L) {
      memcpy(processasn.FTN_OPEN_Access,"SEQUENTIAL",strlen("SEQUENTIAL"));
      processasn.Flag_FTN_ACCESS  =  1L;
    }
    if(processasn.Flag_FTN_FORM == 0L) {
      memcpy(processasn.FTN_OPEN_Form,"FORMATTED",strlen("FORMATTED"));
      processasn.Flag_FTN_FORM  =  1L;
    }
    if(processasn.Flag_FTN_RECL == 0L) {
      processasn.FTN_OPEN_Recl  = processasn.Value_IN;
      processasn.Flag_FTN_RECL  =  1L;
    }
  }

  if(processasn.Flag_BRKSHRT || processasn.Flag_SYSEXC || 
     processasn.Flag_MARPAD  || processasn.Flag_CON) {
    if(processasn.Flag_FTN_STATUS == 0L) {
      memcpy(processasn.FTN_OPEN_Status,"OLD",strlen("OLD"));
      processasn.Flag_FTN_STATUS  =  1L;
    }
    if(processasn.Flag_FTN_ACCESS == 0L) {
      memcpy(processasn.FTN_OPEN_Access,"SEQUENTIAL",strlen("SEQUENTIAL"));
      processasn.Flag_FTN_ACCESS  =  1L;
    }
    if(processasn.Flag_FTN_FORM == 0L) {
      memcpy(processasn.FTN_OPEN_Form,"FORMATTED",strlen("FORMATTED"));
      processasn.Flag_FTN_FORM  =  1L;
    }
    if(processasn.Flag_FTN_RECL == 0L) {
      processasn.FTN_OPEN_Recl  =  80L;
      processasn.Flag_FTN_RECL  =  1L;
    }
  }

  if(processasn.Flag_PR || processasn.Flag_LOG) {
    if(processasn.Flag_FTN_STATUS == 0L) {
      memcpy(processasn.FTN_OPEN_Status,"UNKNOWN",strlen("UNKNOWN"));   //tt62010
      processasn.Flag_FTN_STATUS  =  1L;
    }
    if(processasn.Flag_FTN_ACCESS == 0L) {
      memcpy(processasn.FTN_OPEN_Access,"SEQUENTIAL",strlen("SEQUENTIAL"));
      processasn.Flag_FTN_ACCESS  =  1L;
    }
    if(processasn.Flag_FTN_FORM == 0L) {
      memcpy(processasn.FTN_OPEN_Form,"PRINT",strlen("PRINT"));
      processasn.Flag_FTN_FORM  =  1L;
    }
    if(processasn.Flag_FTN_RECL == 0L) {
      processasn.FTN_OPEN_Recl  =  132L;
      processasn.Flag_FTN_RECL  =  1L;
    }
  }


  /*--------------------------------------------------------------------------------*/
  /* Set any of the FORTRAN options that may not be set                             */
  /*--------------------------------------------------------------------------------*/

  if(processasn.Flag_FTN_STATUS == 0L) {
    memcpy(processasn.FTN_OPEN_Status,"UNKNOWN",strlen("UNKNOWN"));
    processasn.Flag_FTN_STATUS  =  1L;
  }

  if(processasn.Flag_FTN_ACCESS == 0L) {
    memcpy(processasn.FTN_OPEN_Access,"SEQUENTIAL",strlen("SEQUENTIAL"));
    processasn.Flag_FTN_ACCESS  =  1L;
  }

  if(processasn.Flag_FTN_FORM == 0L) {
    if(processasn.Flag_FTN_SEQUENTIAL != 0L) {
      memcpy(processasn.FTN_OPEN_Form,"FORMATTED",strlen("FORMATTED"));
      processasn.Flag_FTN_FORMATTED  =  1L;
    }
    else if(processasn.Flag_FTN_DIRECT != 0L) {
      memcpy(processasn.FTN_OPEN_Form,"UNFORMATTED",strlen("UNFORMATTED"));
      processasn.Flag_FTN_UNFORMATTED  =  1L;
    }
    else {
      memcpy(processasn.FTN_OPEN_Form,"FORMATTED",strlen("FORMATTED"));
      processasn.Flag_FTN_FORMATTED  =  1L;
    }
    processasn.Flag_FTN_FORM  =  1L;
  }

  /*--------------------------------------------------------------------------------*/
  /* Check for a colon in the TCP or LOCAL Flags have not been set                  */
  /*--------------------------------------------------------------------------------*/

  if((processasn.Flag_CLIENT == 0) && (processasn.Flag_SERVER == 0)   &&
     (processasn.Flag_LOCAL  == 0)                                  )    {
    if(strchr((char *)processasn.Value_Filename,':') != (char *) NULL) {
      processasn.Flag_SERVER = 1L;
    }
  }

  return;
}


/*==================================================================================*/
/* Get_ASNOption()                                                                  */
/*==================================================================================*/

static int      Get_ASNOption(UCHAR * rdbuffer, int      * offset, int       length,
                              UCHAR * option)
{
  int       status;                                  /* Working status              */
  int       rtnstatus;                               /* Working status              */
  int       optoffset=0;                             /* Saved offset value          */
  int       valoffset;                               /* Saved offset value          */
  int       value;                                   /* Working value variable      */
  int       optlen;                                  /* Length of the option        */

  rtnstatus =  0L;                                   /* Say all is okay             */
  (*offset) = fnblnk(rdbuffer,(*offset),length);     /* Move to non-blank character */
  status    = GetOptions(rdbuffer,offset,length,option);/* Extract the option       */
  if(status >= 0L) {                                 /* We have an option           */
    optoffset  = status;                             /* Save the new offset         */
    valoffset  = status;                             /* Save the new offset         */
    optlen     = strlen((char *)option);                     /* Get the length of the option*/
    status     = Option_Assign(option);              /* Detimine which option       */
    if(status >= 0L) {   
      (*offset) = optoffset;
      switch(status) {
        case  ASN_CON           :
        case  ASN_TTY           :
                                  processasn.Flag_CON  =  1L;
                                  processasn.Value_DCOD = DCOD_TTY;
                                  break;
        case  ASN_PR            :
        case  ASN_LOG           :
                                  processasn.Flag_PR  =  1L;
                                  processasn.Value_DCOD = DCOD_PRINTER;
                                  break;
        case  ASN_CO            :
        case  ASN_RANDOM        :
                                  processasn.Flag_CO  =  1L;
                                  processasn.Value_DCOD = DCOD_CONTIG;
                                  (*offset) = fnblnk(rdbuffer,(*offset),length);
                                  valoffset = (*offset);
                                  (*offset) = fblnk(rdbuffer,(*offset),length);
                                  value     = getvalue(rdbuffer,valoffset,(*offset)-1L);
                                  if(value < 0L) {
                                    rtnstatus = -2L;
                                    status    = -2L;
                                  }
                                  else {
                                    processasn.Value_CO   = value;
                                  }
                                  break;
        case  ASN_IN            :
        case  ASN_SEQUENTIALLY  :
                                  processasn.Flag_IN  =  1L;
                                  processasn.Value_DCOD = DCOD_INDEX;
                                  (*offset) = fnblnk(rdbuffer,(*offset),length);
                                  valoffset = (*offset);
                                  (*offset) = fblnk(rdbuffer,(*offset),length);
                                  value     = getvalue(rdbuffer,valoffset,(*offset)-1L);
                                  if(value < 0L) {
                                    rtnstatus = -2L;
                                    status    = -2L;
                                  }
                                  else {
                                    processasn.Value_IN = value;
                                  }
                                  break;
        case  ASN_BRKSHRT       :
                                  processasn.Flag_BRKSHRT  =  1L;
                                  processasn.Value_DCOD = DCOD_BRKSHRT;
                                  break;
        case  ASN_SYSEXC        :
                                  processasn.Flag_SYSEXC  =  1L;
                                  processasn.Value_DCOD = DCOD_SYSEXC;
                                  break;
        case  ASN_MARPAD        :
                                  processasn.Flag_MARPAD  =  1L;
                                  processasn.Value_DCOD = DCOD_MARPAD;
                                  break;
        case  ASN_CLIENT        :
                                  processasn.Flag_CLIENT  =  1L;
                                  break;
        case  ASN_SERVER        :
        case  ASN_TCP           :
                                  processasn.Flag_SERVER  =  1L;
                                  break;
        case  ASN_LOCAL         :
                                  processasn.Flag_LOCAL  =  1L;
                                  break;
        case  ASN_RECL          :
                                  (*offset) = fnblnk(rdbuffer,(*offset),length);
                                  valoffset = (*offset);
                                  (*offset) = fblnk(rdbuffer,(*offset),length);
                                  value     = getvalue(rdbuffer,valoffset,(*offset)-1L);
                                  if(value < 0L) {
                                    rtnstatus = -2L;
                                    status    = -2L;
                                  }
                                  else {
                                    processasn.Flag_FTN_RECL  =  1L;
                                    processasn.FTN_OPEN_Recl  =  value;
                                  }
                                  break;
        case  ASN_TIE           :
                                  (*offset) = fnblnk(rdbuffer,(*offset),length);
                                  valoffset = (*offset);
                                  (*offset) = fblnk(rdbuffer,(*offset),length);
                                  value     = getvalue(rdbuffer,valoffset,(*offset)-1L);
                                  if(value < 0L) {
                                    rtnstatus = -1L;
                                    status    = -1L;
                                  }
                                  else {
                                    if((value < 0) || (value > (MAXIMUM_LU-1L))) {
                                      rtnstatus = -3L;
                                      status    = -3L;
                                    }
                                    else {
                                      processasn.Value_Tie = value;
                                      processasn.Flag_Tie   =  1L;
                                    }
                                  }
                                  break;
        case  ASN_TTYM          :
                                  processasn.Flag_CON  =  1L;
                                  processasn.Value_DCOD = DCOD_TTYM;
                                  break;
        case  ASN_MIRROR        :
                                  processasn.Flag_MIRROR  =  1L;
                                  break;
        case  FTN_OLD           :
                                  processasn.Flag_FTN_OLD     =  1L;
                                  processasn.Flag_FTN_STATUS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Status,option,strlen((char *)option));
                                  break;
        case  FTN_NEW           :
                                  processasn.Flag_FTN_NEW     =  1L;
                                  processasn.Flag_FTN_STATUS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Status,option,strlen((char *)option));
                                  break;
        case  FTN_SCRATCH       :
                                  processasn.Flag_FTN_SCRATCH  =  1L;
                                  processasn.Flag_FTN_STATUS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Status,option,strlen((char *)option));
                                  break;
        case  FTN_UNKNOWN       :
                                  processasn.Flag_FTN_UNKNOWN  =  1L;
                                  processasn.Flag_FTN_STATUS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Status,option,strlen((char *)option));
                                  break;
        case  FTN_APPEND        :
                                  processasn.Flag_FTN_APPEND  =  1L;
                                  processasn.Flag_FTN_ACCESS  =  1L;                                //tt62010
                                  memcpy(processasn.FTN_OPEN_Access,option,strlen((char *)option)); //tt62010
                                  break;
        case  FTN_SEQUENTIAL    :
                                  processasn.Flag_FTN_SEQUENTIAL  =  1L;
                                  processasn.Flag_FTN_ACCESS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Access,option,strlen((char *)option));
                                  break;
        case  FTN_DIRECT        :
                                  processasn.Flag_FTN_DIRECT  =  1L;
                                  processasn.Flag_FTN_ACCESS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Access,option,strlen((char *)option));
                                  break;
        case  FTN_FORMATTED     :
                                  processasn.Flag_FTN_FORMATTED  =  1L;
                                  processasn.Flag_FTN_FORM  =  1L;
                                  memcpy(processasn.FTN_OPEN_Form,option,strlen((char *)option));
                                  break;
        case  FTN_UNFORMATTED   :
                                  processasn.Flag_FTN_UNFORMATTED  =  1L;
                                  processasn.Flag_FTN_FORM  =  1L;
                                  memcpy(processasn.FTN_OPEN_Form,option,strlen((char *)option));
                                  break;
        case  FTN_PRINT         :
                                  processasn.Flag_FTN_PRINT  =  1L;
                                  processasn.Flag_FTN_FORM  =  1L;
                                  memcpy(processasn.FTN_OPEN_Form,option,strlen((char *)option));
                                  break;
        case  FTN_READONLY      :
                                  processasn.FTN_OPEN_Readonly  =  1L;
                                  break;
        case  FTN_RENEW         :
                                  processasn.Flag_FTN_RENEW  =  1L;
                                  processasn.FTN_OPEN_Renew  =  1L;
                                  processasn.Flag_FTN_STATUS  =  1L;
                                  memcpy(processasn.FTN_OPEN_Status,"NEW",strlen("NEW"));
                                  break;
        default                 :
                                  status                      = -1L;
                                  rtnstatus                   = -1L;
                                  break;
      }
      if(status >= 0) {           /* No Error being reported                        */
        rtnstatus =  0L;          /* All offsets are okay                           */
      /*(*offset) = valoffset;*/  /* Move after the option and option value         */
      }                           /*                                                */
      else {                      /*                                                */
        if(status == -1L) {       /* Option is not known here - try other tables    */
          (*offset) = optoffset;
        }
        else if(status == -2L) {
          (*offset) = optoffset;
        }
        else if(status == -3L) {
          (*offset) = valoffset;
        }
        else {
          (*offset) = optoffset;
        }
      }                           /*                                                */
    }                             /*                                                */
    else {                        /*                                                */
      (*offset) = optoffset;      /* Insure offset points to string                 */
      rtnstatus = -1L;            /* Invalid option                                 */
    }
  }
  else {
    (*offset)   = optoffset;      /* Insure offset points to string                 */
    rtnstatus   = -1L;            /* No option is present                           */
  }
  return(rtnstatus);
}


/*==================================================================================*/
/* Clear/Reset Common Flags and Variables                                           */
/*==================================================================================*/

static void Process_ASN_Clear_Common(void)
{
  processasn.Value_CO              = -1L;
  processasn.Value_IN              = -1L;
  processasn.Value_DCOD            = -1L;
  processasn.Value_Lu              = -1L;
  processasn.Value_Lu2             = -1L;
  processasn.Value_Tie             = -1L;
  processasn.Value_Recl            = -1L;

  processasn.Flag_CO               =  0L;
  processasn.Flag_IN               =  0L;
  processasn.Flag_BRKSHRT          =  0L;
  processasn.Flag_SYSEXC           =  0L;
  processasn.Flag_MARPAD           =  0L;
  processasn.Flag_PR               =  0L;
  processasn.Flag_CON              =  0L;
  processasn.Flag_LOG              =  0L;

  processasn.Flag_CLIENT           =  0L;
  processasn.Flag_SERVER           =  0L;
  processasn.Flag_LOCAL            =  0L;

  processasn.Flag_MIRROR           =  0L;

  processasn.Flag_Tie              =  0L;

  processasn.Value_Data_Path       =  0L;
  processasn.Value_Rept_Path       =  0L;
  processasn.Value_Mirr_Path       =  0L;

  processasn.Flag_FTN_OLD          =  0L;
  processasn.Flag_FTN_NEW          =  0L;
  processasn.Flag_FTN_SCRATCH      =  0L;
  processasn.Flag_FTN_UNKNOWN      =  0L;
  processasn.Flag_FTN_APPEND       =  0L;
  processasn.Flag_FTN_RENEW        =  0L;

  processasn.Flag_FTN_SEQUENTIAL   =  0L;
  processasn.Flag_FTN_DIRECT       =  0L;
  processasn.Flag_FTN_ACCESS       =  0L;

  processasn.Flag_FTN_FORMATTED    =  0L;
  processasn.Flag_FTN_UNFORMATTED  =  0L;
  processasn.Flag_FTN_PRINT        =  0L;

  processasn.Flag_FTN_STATUS       =  0L;
  processasn.Flag_FTN_FORM         =  0L;
  processasn.Flag_FTN_RECL         =  0L;

  memcpy(processasn.FTN_OPEN_Status,"            ",strlen("            "));
  memcpy(processasn.FTN_OPEN_Access,"            ",strlen("            "));
  memcpy(processasn.FTN_OPEN_Form,  "            ",strlen("            "));
  processasn.FTN_OPEN_Recl         =  0L;
  processasn.FTN_OPEN_Readonly     =  0L;
  processasn.FTN_OPEN_Renew        =  0L;

  return;
}


/*==================================================================================*/
/* Attach the Requested Logical Unit                                                */
/*==================================================================================*/

static int        AttachLU(void)
{
  int             status;                       /* Return Status                    */
  int             lu;                           /* Short hand                       */

  Set_Defaults();                               /* Set the defaults                 */

  lu   =   processasn.Value_Lu;                 /* Get the logical unit             */
  if((lu == 5) || (lu == 6)) {                  /* If lu 5 or 6 then insure they    */
    status = DettachLU_Language(lu);            /* closed first.                    */
  }                                             /*                                  */

  lu   =   processasn.Value_Lu2;                /* Get the logical unit             */
  if((lu == 5) || (lu == 6)) {                  /* If lu 5 or 6 then insure they    */
    status = DettachLU_Language(lu);            /* closed first.                    */
  }                                             /*                                  */

  if((processasn.Flag_SERVER) || (processasn.Flag_CLIENT)) {
    status = AttachLU_TCP();
  }
  else if((processasn.Flag_LOCAL)) {
    status = AttachLU_LOCAL();
  }
  else {
    status = AttachLU_LOCAL();
  }

  if(status == 0L) {
    strcpy((char *)private_data.lu_filename[processasn.Value_Lu],(char *)processasn.Value_Filename);
    if(processasn.Value_Lu2 >= 0L) {
      strcpy((char *)private_data.lu_filename[processasn.Value_Lu2],(char *)processasn.Value_Filename);
    }
  }

  return(status);
}


/*==================================================================================*/
/* Attach the Requested Logical Unit to a TCP/IP Connection                         */
/*                                                                                  */
/* We use the FORTRAN callable tcp_connect_f.c to retain various messages that are  */
/* generated when errors or other events occur.                                     */
/*                                                                                  */
/*==================================================================================*/

static int        AttachLU_TCP(void)
{
  int             rtnstatus;                    /* Return Status                    */
  int             status;                       /* Working Status                   */
  int             host_len;                     /* Length of the host string        */
  int             port_len;                     /* Length of the port string        */
  int             unix_fd;                      /* Unix File Descriptor             */
  int             retries;                      /* Retry Count for Client connects  */
  int             tmdelay;                      /* Time Delay                       */
  int             index;                        /* Working Index                    */
  UCHAR           host[WRK_NAMESIZE];           /* Host String                      */
  UCHAR           port[WRK_NAMESIZE];           /* Host String                      */

  /*--------------------------------------------------------------------------------*/
  /* Split the host and port (service) strings for the TCP calls.                   */
  /*--------------------------------------------------------------------------------*/

  memset(host, (int     ) '\0', WRK_NAMESIZE);
  memset(port, (int     ) '\0', WRK_NAMESIZE);

  status     = get_hostport(processasn.Value_Filename,host,port);

  IF_DEBUG(1)
    fprintf(stderr,"   Host=%s, Port=%s, Status=%d\n",host,port,status);
  END_DEBUG

  if(status == 0L) {                            /* We have a host:port string       */

    /*------------------------------------------------------------------------------*/
    /* Open the TCP/IP connection                                                   */
    /*------------------------------------------------------------------------------*/

    host_len = strlen((char *)host);                    /* Get length of the host string    */
    port_len = strlen((char *)port);                    /* Get length of the port string    */

    if((processasn.Flag_SERVER)) {
      (void) open_tcp_as_server_(&unix_fd,(char *)host,(char *)port,host_len,port_len);
    }
    else if((processasn.Flag_CLIENT)) {
      retries   = ((TCP_CLIENT_RETRY <=   0) ?    1 : TCP_CLIENT_RETRY);
      tmdelay   = ((TCP_CLIENT_DELAY <= 999) ? 1000 : TCP_CLIENT_DELAY);
      for(index=0;index<retries;index++) {
        (void) open_tcp_as_client_(&unix_fd,(char *)host,(char *)port,host_len,port_len);
        if((unix_fd < -1L) || (unix_fd >= 0L)) {
          break;
        }
        (void) millisleep(tmdelay);
        fprintf(stderr,fmtretry,index+1,retries);
      }
    }
    else {
      unix_fd    = -1L;                         /* Indicate an error                */
    }

    /*------------------------------------------------------------------------------*/
    /* Set-up the OS/32 Emulation Subsystem on a good connection                    */
    /*------------------------------------------------------------------------------*/

    if(unix_fd >= 0L) {                         /* The TCP/IP link is established   */

      Set_Lu_FD(  processasn.Value_Lu,TRUE,unix_fd);
      Set_Lu_DCOD(processasn.Value_Lu,processasn.Value_DCOD);
      Set_Lu_RECL(processasn.Value_Lu,processasn.Value_Recl);
      Set_Lu_SIZE(processasn.Value_Lu,processasn.Value_CO);

      /*----------------------------------------------------------------------------*/
      /* Set-up the Dual LU Connections as required                                 */
      /*----------------------------------------------------------------------------*/
 
      if((processasn.Flag_MARPAD) || (processasn.Flag_BRKSHRT) || (processasn.Flag_Tie)) {

        Set_Lu_FD(  processasn.Value_Lu2,TRUE,unix_fd);
        Set_Lu_DCOD(processasn.Value_Lu2,processasn.Value_DCOD);
        Set_Lu_RECL(processasn.Value_Lu2,processasn.Value_Recl);
        Set_Lu_SIZE(processasn.Value_Lu2,processasn.Value_CO);

      }

      fprintf(stderr," Process_ASN : Opened TCP Port : %-29.29s %3d %3d\n",
              processasn.Value_Filename,processasn.Value_Lu,unix_fd);

      rtnstatus  =  0L;                         /* Indicate a good assign           */
    }
    else {

      rtnstatus  = -1L;                         /* Indicate an error with host:port */

    }

  }
  else {

    rtnstatus  = -1L;                           /* Indicate an error with host:port */

  }                                             /*                                  */

  return(rtnstatus);                            /* Return to the caller             */
}                                               /*                                  */


/*==================================================================================*/
/* Attach the Requested Logical Unit to a Local File or Device                      */
/*                                                                                  */
/*==================================================================================*/

static int        AttachLU_LOCAL(void)
{
  int             rtnstatus;                    /* Status to return to the caller   */
  int             status;                       /* Working status                   */
  int             ttyflag;                      /* Flag for tty                     */
  int             unix_fd;                      /* Unix File Descriptor             */
  int             unix_fd2;                     /* Unix File Descriptor             */
  int             zero;                         /* Zero Value (must be set to 0)    */
  int             mirrorfd;                     /* Fd for the mirroring             */
  unsigned char   wrkbuffer[WRK_NAMESIZE];      /* Work buffer                      */
  struct stat     wrkrenew;                     /* Work buffer for renew work       */

  rtnstatus    = 0L;                            /* Indicate a good status           */
  status       = 0L;                            /* Indicate a good status           */

  IF_DEBUG(1)
    fprintf(stderr,"   OPEN_DATA_PATH=<%s>, FileName=<%s>\n",
            processasn.OPEN_DATA_PATH,processasn.Value_Filename);
  END_DEBUG

  if(processasn.OPEN_DATA_PATH[0] != '\0') {    /* We have a data path              */
    if(processasn.Value_Filename[0] != '/') {   /* Not absolute                     */
      memset(wrkbuffer,0x00,WRK_NAMESIZE);      /* Clear it                         */
      strcpy((char *)wrkbuffer,(char *)processasn.OPEN_DATA_PATH); /* Copy the data path            */
      strcat((char *)wrkbuffer,(char *)processasn.Value_Filename); /* Add in the filename           */
      strcpy((char *)processasn.Value_Filename,(char *)wrkbuffer); /* Put the name back in          */
    }                                           /*                                  */
  }                                             /*                                  */
  
  IF_DEBUG(1)
    fprintf(stderr,"   Open File %s on Lu %d\n",processasn.Value_Filename,
                                                processasn.Value_Lu);
  END_DEBUG

  if(processasn.Flag_FTN_RENEW) {
    status = lstat((char *)processasn.Value_Filename,&wrkrenew);
    if(status < 0L) {
      if(errno != ENOENT) {
        status = -2L;
      }
      else {
        status =  0L;
      }
    }
    else {
      if(((wrkrenew.st_mode & S_IFMT) == S_IFREG) ||
         ((wrkrenew.st_mode & S_IFMT) == S_IFLNK) )  {
        status = unlink((char *)processasn.Value_Filename);
        if(status != 0L) {
          status = -3L;
        }
        else {
          status =  0L;
        }
      }
      else {
        status =  -3L;
      }
    }
  }

  if(status == 0L) {
    status       =  AttachLU_Language();        /* Go and attach by Language        */
  }

  if(status == 0L) {

    unix_fd =  GetFD_(&processasn.Value_Lu);    /* Get the associated logical unit  */

    fprintf(stderr," Process_ASN : Opened File     : %-29.29s %3d %3d %3d\n",
            processasn.Value_Filename,processasn.Value_Lu,unix_fd,status);

    if(processasn.Flag_FTN_RENEW) {
      status  = PreFillFile(processasn.Value_Lu);
      if(status < 0L) { 
        fprintf(stderr," *** Error %3d reported during pre-fill of file %s on LU %d.\n",
                       status,processasn.Value_Filename,processasn.Value_Lu);
      } 
    }

    if(processasn.Value_DCOD < 0L) {            /* Set the Device Code if not set   */
      ttyflag = isatty(unix_fd);                /* Check if assignment is a tty     */
      if(ttyflag) {                             /* Is it a tty                      */
        processasn.Value_DCOD = DCOD_TTY;       /* Set device code to TTY           */
      }                                         /*                                  */
      else {                                    /* Not a tty so use INDEX filetype  */
        processasn.Value_DCOD = DCOD_INDEX;     /* Set device code to INDEX         */
      }                                         /*                                  */
    }                                           /*                                  */

    if (processasn.Value_Recl > 0) {            // 78918 need this to be conditional.
      dskfilesize(processasn.Value_Lu, processasn.Value_DCOD,  processasn.Value_Recl,
                                                            &processasn.Value_CO);
    } 
    Set_Lu_FD(  processasn.Value_Lu,FALSE,unix_fd);
    Set_Lu_DCOD(processasn.Value_Lu,processasn.Value_DCOD);
    Set_Lu_RECL(processasn.Value_Lu,processasn.Value_Recl);
    Set_Lu_SIZE(processasn.Value_Lu,processasn.Value_CO);
    
#if 0
    fprintf(stderr,"Lu %5d, Dcode = %5d, Size = %5d, Recl = %5d\n",
            processasn.Value_Lu,processasn.Value_DCOD,processasn.Value_CO,
            processasn.Value_Recl);
#endif

    if(processasn.Value_Lu2 >= 0L) {            /* Secondary Logical Unit           */
      unix_fd2 =  GetFD_(&processasn.Value_Lu2);/* Get the associated logical unit  */

      Set_Lu_FD(  processasn.Value_Lu2,FALSE,unix_fd2);
      Set_Lu_DCOD(processasn.Value_Lu2,processasn.Value_DCOD);
      Set_Lu_RECL(processasn.Value_Lu2,processasn.Value_Recl);
      Set_Lu_SIZE(processasn.Value_Lu2,processasn.Value_CO);

    }

    zero   = 0L;

    if(processasn.Flag_BRKSHRT != 0L) {
      ttyb_init_(&processasn.Value_Lu,&zero);
    }

    if(processasn.Flag_SYSEXC != 0L) {
      ttys_init_(&processasn.Value_Lu,&zero);
    }

    if((processasn.Flag_MARPAD != 0L) || (processasn.Value_DCOD == DCOD_TTYM)) {
      ttym_init_(&processasn.Value_Lu,&zero);
    }

    if(processasn.Flag_MIRROR > 0L) {
      if(processasn.Value_Mirr_Path > 0L) {
        IF_DEBUG(2)
          fprintf(stderr,"   Mirroring %s in directory %s.\n",
                             processasn.Value_Filename,
                             processasn.OPEN_MIRR_PATH);
        END_DEBUG
        mirrorfd = mirrorlu_(processasn.OPEN_MIRR_PATH,
                             processasn.Value_Filename,
                             strlen((char *)processasn.OPEN_MIRR_PATH),
                             strlen((char *)processasn.Value_Filename));   
        if(mirrorfd >= 0L) {
          private_data.lu_mirror[processasn.Value_Lu] = mirrorfd;
        }
        else {
          fprintf(stderr,fmtmirr0,processasn.Value_Filename);
        }
      }
      else {
        fprintf(stderr,fmtmirr1);
        fprintf(stderr,fmtmirr2,processasn.Value_Filename);
      }
    }

  }
  else {

    rtnstatus = -1L;

    fprintf(stderr," Process_ASN : Unable to assign %-29.29s to Lu %3d, Status %5d\n",
            processasn.Value_Filename,processasn.Value_Lu,status);

  }

  return(rtnstatus);
}


/*==================================================================================*/
/* Prefill a "CONTIGUOUS" file                                                      */
/*==================================================================================*/

#define  NSECTORS   8L

static int        PreFillFile(int      lu)
{
  int             status;                    /* General Purpose Status              */
  int             unixfd;                    /* The Unix File Descriptor            */
  int             index;                     /* Working Index                       */
  int             increment;                 /* Number of sectors to increment      */
  int             count;                     /* Count of sectors left to write      */
  size_t          nbytes;                    /* Number of bytes to write            */
  ssize_t         nwritten;                  /* Number of bytes written             */
  off_t           newloc;                    /* Old location of the file pointer    */
  UCHAR           sectors[NSECTORS * SECTOR];/* Work Buffer for clearing sectors    */

  if(processasn.Value_DCOD == DCOD_CONTIG) { /* We have a contiguous file           */

    unixfd = GetFD_(&lu);                    /* Get the unix file descriptor        */
    if(unixfd < 0L) {                        /* No fd found                         */
      status = -2L;                          /* Report the error                    */
      return(status);   /* <<<<<<<<<<<<< */  /* Return to the caller                */
    }                                        /*                                     */

    memset(sectors,0x00,NSECTORS * SECTOR);  /* Clear the work buffers              */

    count    = processasn.Value_CO;          /* Number of sectors to write          */

    fprintf(stderr," Process_ASN :  Creating %d sectors for file %-29.29s\n",
                   count,processasn.Value_Filename);

    status = 0L;                             /* Default to good status              */

    for(index=0;index<processasn.Value_CO;) {/* Write sectors in blocks of NSECTORS */

      errno   = 0L;

      if(count >= NSECTORS) {                /* Have NSECTORS or more sectors ?     */
        increment = NSECTORS;                /* Yes, Write NSECTORS sectors         */
      }                                      /*                                     */
      else {                                 /* Less than NSECTORS sectors ?        */
        increment = count;                   /* Yes, Write only what is left        */
      }                                      /*                                     */
      nbytes    = increment * SECTOR;        /* Number of bytes to write            */

      nwritten  = write(unixfd,sectors,nbytes); /* Write the bytes                  */

      if(nwritten != nbytes) {               /* Error on the prefill                */
        status = -1L;                        /* Indicate an error occurred          */
        break;                               /*                                     */
      }                                      /*                                     */

      index    += increment;                 /* Count the amount of data written    */
      count    -= increment;                 /* Amount left to write                */

    }                                        /*                                     */

    newloc = lseek(unixfd,(off_t) 0L,SEEK_SET); /* Rewind the file                  */

  }                                          /* End of processasn.Value_DCOD        */

  else {                                     /* Not a Contiguous File               */

    status = 0L;                             /* Default to good status              */

  }                                          /* End of Not a Contiguous File        */

  return(status);                            /* Return to the caller                */
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
