/************************************************************************************/
/*                                                                                  */
/* process_base.c      RUN-FILE support (Base Processing Code)            R00-00.00 */
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
/* R00-00.00  DRLJr        13/Apr/1999  process_common.c split into 2 pieces        */
/* R01-00.00  jeffsun      13/Feb/2001  Sun C requires null statement with label.   */
// tt78919    jeffs        21-Apr-2014  Add init_open_store() call for data struct
//                                      initialization of file number storage.
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* Various Macros                                                                   */
/*==================================================================================*/


#define  DSPERROR(offset)                                                        \
    /*=======================================================================*/  \
    if(Errors == 0L) {                       /* Output initial error message */  \
      fprintf(stderr,fmt10250,processasn.CALL_FileName[processasn.RunDepth]);    \
    }                                        /*                              */  \
    else {                                   /*                              */  \
      fprintf(stderr,fmt10003);              /* Output the error message     */  \
    }                                        /*                              */  \
    fprintf(stderr,fmt10001,rdbuffer);       /* Output the error message     */  \
    errorbuf[offset+0L] = '^';               /* Put the marker               */  \
    errorbuf[offset+1L] = '\0';              /* Put the marker               */  \
    fprintf(stderr,fmt10002,errorbuf);       /* Write the marker             */  \
    errorbuf[offset+0L] = ' ';               /* Put the marker               */  \
    errorbuf[offset+1L] = ' ';               /* Put the marker               */  \
    /*=======================================================================*/

/*==================================================================================*/
/* Error Message Formats                                                            */
/*==================================================================================*/

#define  FORMAT(name,text)  static char name[] = { text }

//FORMAT(fmt10000," Process_ASN : I/O Error detected while reading from %s\n");
FORMAT(fmt10001," Process_ASN : \"%s\"\n");
FORMAT(fmt10002," Process_ASN :  %s\n");
FORMAT(fmt10003," Process_ASN :    \n");
FORMAT(fmt10010,                                                              \
  " Process_ASN : Unable to find an Assignment Control File.  Base name is %s.\n");
FORMAT(fmt10020," Process_ASN : Unknown Keyword Error on record %3d, Offset %3d of %s\n");
FORMAT(fmt10025," Process_ASN : Invalid LU on record %3d, Offset %3d of %s\n");
FORMAT(fmt10030," Process_ASN : %3d errors found in the RUN file %s or called files\n");
FORMAT(fmt10040," Process_ASN : Option is unknown or unsupported.\n");
FORMAT(fmt10041," Process_ASN : Required numeric argument for option is missing.\n");
FORMAT(fmt10042," Process_ASN : Required numeric argument is invalid/out-of-range.\n");
FORMAT(fmt10045," Process_ASN : Spurious trailing data on record %3d, Offset %3d\n");
FORMAT(fmt10050," Process_ASN : Options are in Conflict on record %3d\n");
//FORMAT(fmt10060,"%s\n");
//FORMAT(fmt10070," Process_ASN : TCP/LOCAL Filename conflict on record %3d\n");
//FORMAT(fmt10080," Process_ASN : Assignment Control File specification required\n");
//FORMAT(fmt10090," Process_ASN : Unable to open %3d logical units\n");
//FORMAT(fmt10100," Process_ASN : Unable to open CALL File %s, File ignored\n");
//FORMAT(fmt10110," Process_ASN : May not tie an LU (%3d) to itself\n");
FORMAT(fmt10200," Process_ASN : Taskname is required part of the TASK command.\n");
FORMAT(fmt10210,
     " Process_ASN : Taskname contains syntax error or contains other errors.\n");
FORMAT(fmt10211,
     " Process_ASN : Taskname is more than 8 characters.  First 8 characters used.\n");
FORMAT(fmt10212,
     " Process_ASN : TASK taskname specified is <%s>.  Using taskname <%8.8s>.\n");
FORMAT(fmt10220," Process_ASN : Directory Path is required with command.\n");
FORMAT(fmt10221," Process_ASN : File or Device name is required with command.\n");
FORMAT(fmt10222," Process_ASN : Logical Unit Number is required with command.\n");
FORMAT(fmt10225," Process_ASN : Base Run-Filename is required with command.\n");
FORMAT(fmt10230," Process_ASN : Directory Path is must be less than %d bytes.\n");
FORMAT(fmt10240,                                                                 \
     " Process_ASN : WARNING : Directory Path \"%s\" can not be located/accessed.\n");
FORMAT(fmt10250," Process_ASN : Error(s) Found in Run File %s.\n");
FORMAT(fmt10260,                                                                 \
     " Procees_ASN : Maximum CALL depth exceeded on record %3d, Offset %3d of %s\n");
FORMAT(fmt10270,                                                                 \
     " Procees_ASN : Command in Run File %s (Record %3d, Offset %3d)\n");
FORMAT(fmt10280," Process_ASN : Internal Error/No Support\n");
FORMAT(fmtmirr0," Process_ASN : Mirroring of File/Device %s failed - Error ignored.\n");
FORMAT(fmtmirr1," Process_ASN : Mirror path not set or is in not in correct order.\n");
FORMAT(fmtmirr2," Process_ASN : File/Device %s will not be mirrored.\n");
FORMAT(fmt10300,
     " Process_ASN : Only one PRIMARY and BACKUP command is allowed per Run-Files.\n");
FORMAT(fmtdbg01," Process_ASN : Debug Flag set to level %d.\n");
FORMAT(fmtdbg02,
       " Process_ASN : Invalid Debug Level/String, Debug Flag set to level %d.\n");
FORMAT(fmtretry," Process_ASN : Retrying TCP Client Connection (Attempt %d of %d)\n");

/*==================================================================================*/
/* Global Work Variables                                                            */
/*==================================================================================*/

static  UCHAR      rdbuffer[WRK_NAMESIZE];         /* I/O Buffer                    */
static  UCHAR      errorbuf[WRK_NAMESIZE];         /* I/O Buffer                    */
static  FILE     * Stream[RUN_DEPTH];              /* Stream to read entries        */
static  int        Record[RUN_DEPTH];              /* Current Record                */
static  int        Errors;                         /* Error Counter                 */
static  int        ErrorsOpen;                     /* Open Error Counters           */

int      fnblnk(unsigned char * buffer, int        index, int        length);
int      fblnk_(unsigned char * buffer, int      * index, int        length);
int      fblnk(unsigned char * buffer, int        index, int        length);
void dbg_queue_write_(int      * flag);
int      check_tname_(char * tname, int      tname_len);
int      getvalue(unsigned char * buffer, int        index, int        jndex);
void initialize_processasn(void);
void init_copen_store(void);                            //78919 open ftoc

/*==================================================================================*/
/* Common Process Assign Processing                                                 */
/*==================================================================================*/

static void Process_Asn_Common(UCHAR * BaseName, int      * rtnstatus)
{
  int            status;                           /* Working Status                */
  int            index;                            /* Working Index                 */
  int            offset;                           /* Current Offset of the buffer  */
  int            length;                           /* Current length of the buffer  */
  int            command;                          /* Command that is to be parsed  */

  /*================================================================================*/
  /* Open the Run File                                                              */
  /*================================================================================*/

  processasn.RunDepth     = 0L;                    /* Set the Run File Depth to 0   */

  status       = AssignRunFile(BaseName,&Stream[processasn.RunDepth]);/* Assign file*/
  if(status != 0) {                                /* Go and Report Error           */
    goto Assign_Error;                             /* Report Assignment error       */
  }                                                /*                               */

  /*================================================================================*/
  /* Clear the Mapping Arrays and other Data Space                                  */
  /*================================================================================*/

  private_data.my_slot            = FALSE;
  private_data.is_sysexc            = FALSE;
  memset(private_data.task_desk_info, ' ', TASKNAMELEN); 
  private_data.number_cmd_args      = 0;
  private_data.first_task_arg       = 0;
  memset(private_data.base_task_name, ' ', TASKNAMELEN);
  private_data.termtype             = ' ';
  private_data.termtype_tcp         = ' ';
  private_data.desk                 = ' ';
  private_data.program              = ' ';
  private_data.port                 = 0;

  memset(private_data.termid, ' ', 80);
  memset(private_data.device_name, ' ', 80);

  for(index=0;index<=MAXIMUM_LU;index++) {
    private_data.lu_to_fd[index]    =   -1L;
    private_data.lu_is_tcp[index]   = FALSE;
    private_data.lu_dcod[index]     =   -1L;
    private_data.lu_recl[index]     =   -1L;
    private_data.lu_size[index]     =   -1L;
    private_data.lu_mirror[index]   =   -1L;
    memset(private_data.lu_filename[index] , 0x0, WRK_NAMESIZE);
  }

  private_data.hostport_flag = HOSTPORT_NEITHER;
  memset(private_data.hostport_name , 0x0, WRK_NAMESIZE);
  private_data.hostport_delay = 0;

  memset(processasn.OPEN_DATA_PATH,(int     )  0L, sizeof(processasn.OPEN_DATA_PATH));
  memset(processasn.OPEN_REPT_PATH,(int     )  0L, sizeof(processasn.OPEN_DATA_PATH));
  memset(processasn.OPEN_MIRR_PATH,(int     )  0L, sizeof(processasn.OPEN_DATA_PATH));

  memset(errorbuf,(int     ) ' ',WRK_NAMESIZE);
  errorbuf[WRK_NAMESIZE-1L] = '\0';

  init_copen_store();                            //78919 open ftoc

  /*================================================================================*/
  /* Prepare for RUN-FILE Processing                                                */
  /*================================================================================*/

  Record[processasn.RunDepth]   =  0L;              /* Set Record Counter to 0      */
  Errors                        =  0L;              /* Indicate no errors           */
  ErrorsOpen                    =  0L;              /* Indicate no open errors      */


  /*================================================================================*/
  /* Read and Parse the RUN-FILE                                                    */
  /*================================================================================*/

  for(;;) {                                         /* Read Processing Loop         */

    /*==============================================================================*/
    /* Read a Record from the RUN-FILE or Current CALL file                         */
    /*==============================================================================*/

    status = ReadASNFile(Stream[processasn.RunDepth],rdbuffer,WRK_NAMESIZE-2L); 
    if(status == -1L) {                             /* End-of-File detected         */
      status = CloseASNFile(Stream[processasn.RunDepth]); /* Close the file         */
      if(processasn.RunDepth == 0L) {               /* EOF on primary file          */
        break;                                      /* Exit Read Loop               */
      }                                             /*                              */
      else {                                        /* EOF on a CALL file           */
        processasn.RunDepth--;                      /* Shift Back One Level         */
      }                                             /*                              */
    }                                               /*                              */
  /*fprintf(stderr,"%s\n",rdbuffer);*/

    /*==============================================================================*/
    /* Increment the record and check for comments                                  */
    /*==============================================================================*/
 
    (Record[processasn.RunDepth])++;                /* Increment the record counter */
    offset       =  0L;                             /* Set the offset to 0          */
    length       =  strlen((char *)rdbuffer);       /* Get the length of the buffer */
    offset       =  fnblnk(rdbuffer,offset,length); /* Skip to the first non-blank  */
    if(offset >= length) {                          /* Completely blank record      */
      goto get_next_record;                         /* Get the next record          */
    }                                               /*                              */
    status = Check_CMNT(rdbuffer,offset,length);    /* Is this the comment chars.   */
    if(status == 1L) {                              /* Comment Record               */
      goto get_next_record;                         /* Get the next record          */
    }                                               /*                              */

    /*==============================================================================*/
    /* Check which keyword we are one                                               */
    /*==============================================================================*/
 
    status  = GetKeyWord(rdbuffer,&offset,length);  /* Identify the keyword         */
    if(status < 0L) {                               /* Keyword Error                */
      goto bad_keyword;                             /* Report the keyword error     */
    }                                               /*                              */
    command = status;                               /* Set the command variable     */
  /*DSPERROR(offset);*/


    /*==============================================================================*/
    /* Process the command line                                                     */
    /*==============================================================================*/
 
IF_DEBUG(1)
    fprintf(stderr,"Command found is %5d\n",command);
END_DEBUG

    switch(command) {          /* Process the command                               */

      case CMD_ASSIGN       :
                               status = Call_AsnFile(rdbuffer,&offset,length,command);
                               if(status !=  0L) {
                                 Errors++;
                                 break;
                               }
                               status = AttachLU();
                               if(status !=  0L) {
                                 ErrorsOpen++;
                                 break;
                               }
                               break;

      case CMD_TASK         :  /* Process a TASK Command ...........................*/
                               
                               status = Cmd_Task(rdbuffer,&offset,length);
                               if(status !=  0L) {
                                 Errors++;
                               }
                               break;
      case CMD_DEBUGQUEUE   :
                               status = 1L;
                               dbg_queue_write_(&status);
                               status = 0L;
                               break;
      case CMD_CALL         :
                               status = Call_RunFile(rdbuffer,&offset,length,command);
                               break;
      case CMD_DATA_PATH    :
                               status = Cmd_GetPath(rdbuffer,&offset,length,command,
                                                    processasn.OPEN_DATA_PATH,256L);
                               if(status == -3L) {  /* -3 is just a warning  */
                                 status = 0L;
                               }
                               break;
      case CMD_REPORT_PATH  :
                               status = Cmd_GetPath(rdbuffer,&offset,length,command,
                                                    processasn.OPEN_REPT_PATH,256L);
                               if(status == -3L) {  /* -3 is just a warning  */
                                 status = 0L;
                               }
                               break;
      case CMD_MIRROR       :
                               status = Cmd_GetPath(rdbuffer,&offset,length,command,
                                                    processasn.OPEN_MIRR_PATH,256L);
                               if(status == -3L) {  /* -3 is just a warning  */
                                 status = 0L;
                               }
                               break;

      case CMD_PRIMARY      :
                               processasn.Flag_Primary = 1L;
                               if((processasn.Flag_Primary > 0L) &&
                                  (processasn.Flag_Backup  > 0L)) {
                                 status = -1L;
                                 fprintf(stderr,fmt10300);
                               }
                               else {
                                 status = Cmd_GetPath(rdbuffer,&offset,length,command,
                                                      processasn.Value_HostPort,256L);
                                 if(status == 0L) {
                                   private_data.hostport_flag = HOSTPORT_PRIMARY;
                                   strcpy((char *)private_data.hostport_name,(char *)processasn.Value_HostPort);
                                   status = Get_Numeric(rdbuffer,&offset,length);
                                   if(status >= 0) {
                                     processasn.Value_HostPort_Delay = status;
                                     private_data.hostport_delay     = status;
                                     status = 0;
                                   }
                                 }
                               }
                               IF_DEBUG(2)
                                 fprintf(stderr,"Primary : Delay = %5d, Host/Port = <%s>\n",
                                         private_data.hostport_delay,
                                         private_data.hostport_name);
                               END_DEBUG
                               break;
      case CMD_BACKUP       :
                               processasn.Flag_Backup  = 1L;
                               if((processasn.Flag_Primary > 0L) &&
                                  (processasn.Flag_Backup  > 0L)) {
                                 status = -1L;
                                 fprintf(stderr,fmt10300);
                               }
                               else {
                                 status = Cmd_GetPath(rdbuffer,&offset,length,command,
                                                      processasn.Value_HostPort,256L);
                                 if(status == 0L) {
                                   private_data.hostport_flag = HOSTPORT_BACKUP;
                                   strcpy((char *)private_data.hostport_name,(char *)processasn.Value_HostPort);
                                   status = Get_Numeric(rdbuffer,&offset,length);
                                   if(status >= 0) {
                                     processasn.Value_HostPort_Delay = status;
                                     private_data.hostport_delay     = status;
                                     status = 0;
                                   }
                                 }
                               }
                               IF_DEBUG(2)
                                 fprintf(stderr,"Backup  : Delay = %5d, Host/Port = <%s>\n",
                                         private_data.hostport_delay,
                                         private_data.hostport_name);
                               END_DEBUG
                               break;
      case CMD_DEBUG        :
                               status = Get_Debug_Option(rdbuffer,&offset,length);
                               break;
      default               :
                               break;

    }


    /*==============================================================================*/
    /* Check the read buffer to detirmine if an error has occurred                  */
    /*==============================================================================*/
 
    if((status == 0L) && (command != CMD_DEBUG)) {
      status = Check_EOL(rdbuffer,&offset,length);
      if(status) {
        DSPERROR(offset);  
        fprintf(stderr,fmt10045,Record[processasn.RunDepth],offset+1L);
        Errors++;
        status = -1L;
      }
    }

    if(status != 0L) {                              /* Errors have occurred         */
      goto complete_errors;                         /* Finish the error processing  */
    }                                               /*                              */

    /*==============================================================================*/
    /* Go and Process the next record                                               */
    /*==============================================================================*/
 
    goto get_next_record;                           /* Get the next record          */
    
    /*==============================================================================*/
    /* Write error message and location of error                                    */
    /*==============================================================================*/

    bad_keyword :                                   /* Generate error for keywords  */

    DSPERROR(offset);  
    fprintf(stderr,fmt10020,Record[processasn.RunDepth],offset+1L,
                   processasn.CALL_FileName[processasn.RunDepth]);

    goto complete_errors;                           /* Finish the error processing  */

    /*==============================================================================*/
    /* Complete error code                                                          */
    /*==============================================================================*/

    complete_errors :                               /* Generate error for keywords  */

    Errors++;                                       /* Increment the error count    */

    goto get_next_record;                           /* Get the next record          */

    /*==============================================================================*/
    /* End of the for(;;) loop                                                      */
    /*==============================================================================*/

    get_next_record :                               /* Get the next record          */
       ;           /*Sun C requires a null statement with the label.jeffsun-02/08/01*/

  }                                                 /* End of the for(;;) loop      */


  /*================================================================================*/
  /* Check for errors being reported                                                */
  /*================================================================================*/

  if(Errors) {                                     /* Errors reported ?             */
    (*rtnstatus) = -1L;                            /* Indicate errors               */
    goto Return_To_Caller;                         /* Go and Return to caller       */
  }                                                /*                               */
    
  /*================================================================================*/
  /* Return with a good status                                                      */
  /*================================================================================*/

  (*rtnstatus) = 0L;                               /* Report a good status          */
  goto Return_To_Caller;                           /* Go and Return to caller       */

  /*================================================================================*/
  /* An assignment error occurred                                                   */
  /*================================================================================*/

  Assign_Error :                                   /* Report Assignment Error       */

  fprintf(stderr,fmt10010,BaseName);               /* Generate the error            */
  (*rtnstatus) = -1L;                              /* Report a bad status           */
  goto Return_To_Caller;                           /* Go and Return to caller       */

  /*================================================================================*/
  /* Exit Program With An End-Of-Task Code 254                                      */
  /*================================================================================*/

  //exit(254L);                                      /* Terminate with an Error Code  */

  /*================================================================================*/
  /* Return to the caller                                                           */
  /*================================================================================*/

  Return_To_Caller:                                /* Return to caller              */

  if(Errors > 0L) {                                /* One or more errors            */
    fprintf(stderr,fmt10030,Errors,processasn.CALL_FileName[processasn.RunDepth]);
  }                                                /*                               */
  else {                                           /* Inform I/O subsystem          */
    initialize_processasn();
    processasn.Invoked = 1L;                       /* Set the flag                  */
  }                                                /*                               */
  IF_DEBUG(2)
    fprintf(stderr,"At-End  : Delay = %5d, Host/Port = <%s>\n",
                   private_data.hostport_delay,
                   private_data.hostport_name);
  END_DEBUG

  return;                                          /* Return to the caller          */
}                                                  /*                               */


/*==================================================================================*/
/* Cmd_Task()                                                                       */
/*==================================================================================*/

static int        Cmd_Task(UCHAR * rdbuffer, int      * offset, int       length)
{
  int             status;                          /* General Purpose Status        */
  int             wrklen;                          /* Length of the buffer          */
  int             index;                           /* Working index                 */
  int             newoffset;                       /* Working index                 */
  unsigned char   buffer[WRK_NAMESIZE];            /* A large work buffer           */
  UCHAR         * adrbuf;                          /* Short hand for CALL_Filename  */

  memset(buffer,0x00,WRK_NAMESIZE);                /* Null the buffer               */
  status = GetOptions(rdbuffer,offset,length,buffer); /* Get the argument           */
  if(status < 0L) {                                /* Not found ?                   */
    status  = -1L;                                 /* Indicate so                   */
  }                                                /*                               */
  else {                                           /* Pad the name as needed        */
    newoffset = status;                            /* Save the new good offset      */
    status    = 0L;                                /* Indicate good status          */
    wrklen    = strlen((char *)buffer);            /* Get length of buffer          */
    index     = 8L - wrklen;                       /* Padding count                 */
    if(index >= 1L) {                              /* Need to pad                   */
      memset(&(buffer[wrklen]),(int     ) ' ',index); /* Pad this many bytes        */
    }                                              /*                               */
    wrklen = strlen((char *)buffer);                       /* Get length of buffer          */
    status = check_tname_((char *)buffer,wrklen);          /* Check the name                */
    if(status != 0L) {                             /* Bad Taskname                  */
      if(status == 3L) {                           /* Name to int                   */
        status = 0L;                               /* Convert to no error           */
        memcpy(private_data.base_task_name,buffer,8L);  /* Store the task name      */
        adrbuf = processasn.CALL_FileName[processasn.RunDepth];
        fprintf(stderr,fmt10211);                  /* And tell what the error is    */
        fprintf(stderr,fmt10212,buffer,private_data.base_task_name);
        fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
      }                                            /*                               */
      else {                                       /* Actual error                  */
        status = -2L;                              /* Indicate failure              */
      }                                            /*                               */
    }                                              /*                               */
    else {                                         /* All is okay                   */
      status = 0L;                                 /*                               */
      memcpy(private_data.base_task_name,buffer,8L);  /* Store the task name        */
    }                                              /*                               */
  }                                                /*                               */

  if(status !=  0L) {                              /* An error was detected         */
    DSPERROR((*offset));                           /* Display Error text            */
    if(status == -1L) {                            /*                               */
      fprintf(stderr,fmt10200);                    /* Task name is required         */
    }                                              /*                               */
    else if(status == -2L) {                       /* Task name is bad              */
      fprintf(stderr,fmt10210);                    /* Report the error              */
    }                                              /*                               */
    else {                                         /* Cover all bases with general  */
      fprintf(stderr,fmt10210);                    /* Syntax error                  */
    }                                              /*                               */
  }                                                /*                               */
  else {                                           /* Good status                   */
    (*offset) = newoffset;                         /* Update the offset             */
  }                                                /*                               */

  return(status);                                  /* Return to the caller          */
}                                                  /*                               */


/*==================================================================================*/
/* Cmd_GetPath()                                                                    */
/*==================================================================================*/

static int        Cmd_GetPath(UCHAR * rdbuffer, int      * offset, int       length,
                              int      cmd,     UCHAR * pathbuff,  int       pathlen)
{
  int             status=0;                          /* General Purpose Status        */
  int             wrklen;                          /* Length of the buffer          */
  int             index;                           /* Working index                 */
  int             newoffset;                       /* Working index                 */
  UCHAR           buffer[WRK_NAMESIZE];            /* Working Buffer                */
  UCHAR         * adrbuf;                          /* Short hand for CALL_Filename  */

  memset(buffer,0x00,WRK_NAMESIZE);                /* Null the buffer               */
IF_DEBUG(3)
  fprintf(stderr,"Entr : %5d = GetOptionASIS(rdbuffer,%5d,%5d,%s)\n",status,(*offset),
                                                                     length,buffer);
END_DEBUG 
  status = GetOptionsASIS(rdbuffer,offset,length,buffer); /* Get the argument       */
IF_DEBUG(3)
  fprintf(stderr,"Exit : %5d = GetOptionASIS(rdbuffer,%5d,%5d,%s)\n",status,(*offset),
                                                                     length,buffer);
END_DEBUG 
  if(status < 0L) {                                /* Not found ?                   */
    status  = -1L;                                 /* Indicate so                   */
  }                                                /*                               */
  else {                                           /* Pad the name as needed        */
    newoffset = status;                            /* Save the new good offset      */
    status    = 0L;                                /* Indicate good status          */
    wrklen    = strlen((char *)buffer);            /* Get length of buffer          */
    if(wrklen >= pathlen) {                        /* Name is to large              */
      status = -2L;                                /* Indicate failure              */
    }                                              /*                               */
    else {                                         /* All is okay                   */
      status =  0L;                                /*                               */
      switch(cmd) {
        case CMD_DATA_PATH     :
        case CMD_REPORT_PATH   :
        case CMD_MIRROR        :
                                 memset(pathbuff,(int     ) '\0',pathlen);
                                 memcpy(pathbuff,buffer,wrklen);
                                 index = access((char *)buffer,F_OK);
                                 if(index < 0L) {
                                   status = -3L;
                                 }
                                 index = strlen((char *)buffer);
                                 if(buffer[index-1L] != '/') {
                                   strcat((char *)buffer,"/");
                                 }
                                 break;
        case CMD_ASSIGN        :
        case CMD_CALL          :
                                 memset(pathbuff,(int     ) '\0',pathlen);
                                 memcpy(pathbuff,buffer,wrklen);
                                 break;

        case CMD_PRIMARY       :
        case CMD_BACKUP        :
                                 memset(pathbuff,(int     ) '\0',pathlen);
                                 memcpy(pathbuff,buffer,wrklen);
                                 IF_DEBUG(3)
                                   fprintf(stderr,"Cmd=%5d, Host/Port=%s\n",
                                                   cmd,pathbuff);
                                 END_DEBUG
                                 break;

        default                :
                                 status = -4L;
                                 break;
      }
    }                                              /*                               */
  }                                                /*                               */


  /*--------------------------------------------------------------------------------*/
  /* Post Extraction Processing                                                     */
  /*--------------------------------------------------------------------------------*/

  IF_DEBUG(3)
    fprintf(stderr,"Cmd=%5d, Status=%5d\n",cmd,status);
  END_DEBUG
  if(cmd != CMD_CALL) {                            /* Missing text handled by CALL  */
    if(status !=  0L) {                            /* Error condition               */
      adrbuf = processasn.CALL_FileName[processasn.RunDepth];
      DSPERROR((*offset));                         /* Display error                 */
      if(status == -1L) {                          /* Path Name missing             */
        if(cmd != CMD_ASSIGN) {                    /* Say Path Name is missing      */
          fprintf(stderr,fmt10220);                /* Report the error              */
        }                                          /*                               */
        else {                                     /* Say Filename/Device Name      */
          fprintf(stderr,fmt10221);                /* Report the error              */
        }                                          /*                               */
      }                                            /*                               */
      else if(status == -2L) {                     /* Path Name is too int          */
        fprintf(stderr,fmt10230,pathlen);          /* Report the error              */
      }                                            /*                               */
      else if(status == -3L) {                     /* Access Error on path          */
        fprintf(stderr,fmt10240,buffer);           /* Report problem                */
        (*offset) = newoffset;                     /* Update offset                 */
      }                                            /*                               */
      else {                                       /*                               */
        fprintf(stderr,fmt10280,buffer);           /* Report problem                */
      }                                            /*                               */
      fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
    }                                              /*                               */
    else {                                         /* Good Status                   */
      (*offset) = newoffset;                       /* Update offset                 */
    }                                              /*                               */
  }                                                /*                               */
  else {                                           /* Processed a CALL Command      */
    if(status == 0L) {                             /* Good status ?                 */
      (*offset) = newoffset;                       /* Update offset else leave alone*/
    }                                              /*                               */
  }                                                /*                               */

  return(status);                                  /* Return to the caller          */
}                                                  /*                               */


/*==================================================================================*/
/* Check_EOL()                                                                      */
/*==================================================================================*/

static int      Check_EOL(UCHAR * rdbuffer, int      * offset, int       length)
{
  int              status;

  (*offset)  = fnblnk(rdbuffer, (*offset), length); /* Move to next non-blank       */
  if((*offset) >= length) {                         /* At the end ?                 */
    status = 0L;                                    /* Yes, good status             */
  }                                                 /*                              */
  else {                                            /* No, check for comments       */
    status = Check_CMNT(rdbuffer,(*offset),length); /* Is this the comment chars.   */
    if(status == 1L) {                              /* Yes, in-line comment         */
      status = 0L;                                  /* Return good status           */
    }                                               /*                              */
    else {                                          /* More data                    */
      status = 1L;                                  /* Indicate more data           */
    }                                               /*                              */
  }                                                 /*                              */
  return(status);                                   /* Return to caller             */
}                                                   /*                              */

/*==================================================================================*/
/* Check_CMNT()                                                                     */
/*==================================================================================*/

static int      Check_CMNT(UCHAR * rdbuffer, int        offset, int       length)
{
  int              status;

  if(rdbuffer[offset] == '*') {                     /* Comment Record               */
    status = 1L;                                    /* Return good status           */
  }                                                 /*                              */
  else if(rdbuffer[offset] == '#') {                /* Comment Record               */
    status = 1L;                                    /* Return good status           */
  }                                                 /*                              */
  else {                                            /* More data                    */
    status = 0L;                                    /* Indicate more data           */
  }                                                 /*                              */
  return(status);                                   /* Return to caller             */
}                                                   /*                              */


/*==================================================================================*/
/* Get_Debug_Option                                                                 */
/*==================================================================================*/

static int      Get_Debug_Option(UCHAR * rdbuffer, int      * offset, int      length)
{
  int           saveoffset;                          /* Save the offset             */
  int           index;                               /* Working index               */
  int           value=0;                             /* Value of debug flag         */
  int           status;                              /* Working status              */
  UCHAR       * adrbuf;                              /* Short hand for CALL_Filename*/
  int      fblnk_(unsigned char * buffer, int      * index, int        length);

  adrbuf = processasn.CALL_FileName[processasn.RunDepth];
  (*offset)  =  fnblnk(rdbuffer,(*offset),length);   /* Move to non-blank character */
  saveoffset = (*offset);                            /* Save the offset             */
  index      = fblnk(rdbuffer,(*offset),length);     /* Locate end+1 of lu number   */
  if((*offset) >= length) {                          /* Missing LU                  */
    processasn.Debug_Flag = value;                   /* Set it                      */
    fprintf(stderr,fmtdbg02,value);                  /* Report the warning          */
    DSPERROR((*offset));                             /* Display error message       */
    fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
    status   =  0L;                                  /* Set the status to errors    */
    value    = +1L;                                  /* Set to level 1              */
  }                                                  /*                             */
  else {                                             /*                             */
    value    = getvalue(rdbuffer,(*offset),index-1L);/* Decode and extract the level*/
    if(value < 0) {                                  /*                             */
      processasn.Debug_Flag = +1L;                   /* Set it                      */
      fprintf(stderr,fmtdbg02,value);                /* Report the warning          */
      DSPERROR((*offset));                           /* Display error message       */
      fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
      status   =  0L;                                /* Set the status to okay      */
    }                                                /*                             */
    else {                                           /*                             */
      processasn.Debug_Flag = value;                 /* Set it                      */
      fprintf(stderr,fmtdbg01,value);                /* Report the state            */
      status   =  0L;                                /* Set the status to okay      */
    }                                                /*                             */
  }                                                  /*                             */
  (*offset)             = index;                     /* Return the offset           */
  return(status);                                    /*                             */
}


/*==================================================================================*/
/* Get_Debug_Option                                                                 */
/*==================================================================================*/

static int      Get_Numeric(UCHAR * rdbuffer, int      * offset, int      length)
{
  int           saveoffset;                          /* Save the offset             */
  int           index;                               /* Working index               */
  int           value;                               /* Value of debug flag         */
  int           status;                              /* Working status              */
  UCHAR       * adrbuf;                              /* Short hand for CALL_Filename*/

  adrbuf = processasn.CALL_FileName[processasn.RunDepth];
  (*offset)  =  fnblnk(rdbuffer,(*offset),length);   /* Move to non-blank character */
  saveoffset = (*offset);                            /* Save the offset             */
  index      = fblnk(rdbuffer,(*offset),length);     /* Locate end+1 of lu number   */
  if((*offset) >= length) {                          /* No Value is present         */
    status   =  0L;                                  /* Set the status to okay      */
  }                                                  /*                             */
  else {                                             /*                             */
    value    = getvalue(rdbuffer,(*offset),index-1L);/* Decode and extract the level*/
    if(value < 0) {                                  /*                             */
      DSPERROR((*offset));                           /* Display error message       */
      fprintf(stderr,fmt10270,adrbuf,Record[processasn.RunDepth],(*offset)+1L);
      status   = -1L;                                /* Set the status to error     */
    }                                                /*                             */
    else {                                           /*                             */
      status   =  value;                             /* Set the status to okay      */
    }                                                /*                             */
  }                                                  /*                             */
  (*offset)             = index;                     /* Return the offset           */
  return(status);                                    /*                             */
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
