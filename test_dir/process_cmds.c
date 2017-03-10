/************************************************************************************/
/*                                                                                  */
/* process_cmds.c    RUN-FILE support for command processing              R00-00.01 */
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
/* R00-00.01  DRLJr        09/Apr/1999  Added some documentation                    */
/*                                                                                  */
/************************************************************************************/


/*==================================================================================*/
/* Special Macros                                                                   */
/*==================================================================================*/

#define   CMDSIZE     64               /* Size of the command work buffer           */
#define   OPTSIZE    256               /* Size of the option  work buffer           */
#define   UCNULL    ((unsigned char *) NULL)

/*==================================================================================*/
/* Static Data Definitions                                                          */
/*                                                                                  */
/* The ASSIGN command must be the first command in the table.  The routine          */
/* alldigits() will be called when the end of the table has been reached and no     */
/* command has been identified.  The routine alldigits() will return 0 if all of    */
/* the bytes are numbers or -1 if one more bytes are not digits.                    */
/*                                                                                  */
/* The table order must match #defines in process_asn.h.                            */
/*                                                                                  */
/*==================================================================================*/

static char   * cmnds[] = {
                         "ASSIGN",          /* [ASSIGN] LU  file-device  options    */
                         "TASK",            /* TASK taskname                        */
                         "DEBUGQUEUE",      /* DEBUGQUEUE                           */
                         "CALL",            /* CALL filename                        */
                         "DATA_PATH",       /* DATA_PATH path-path                  */
                         "REPORT_PATH",     /* REPORT_PATH report-path              */
                         "MIRROR",          /* MIRROR mirror-path                   */
                         "PRIMARY",         /* PRIMARY host:service                 */
                         "BACKUP",          /* PRIMARY host:service                 */
                         "DEBUG",           /* DEBUG n                              */
                        (char *)NULL
                        } ;


/*==================================================================================*/
/* The following table is the options to the ASSIGN command.                        */
/*                                                                                  */
/* The following lists indicate which keywords are mutually exclusive with each     */
/* other.                                                                           */
/*                                                                                  */
/*  ------------ ------------ ------------ ------------ ------------ ------------   */
/*  CO n         RANDOM       CLIENT       OLD          SEQUENTIAL   FORMATTED      */
/*  IN m         SEQUENTIALLY SERVER       NEW          DIRECT       UNFORMATTED    */
/*  CON                       TCP          SCRATCH                   PRINT          */
/*  PR                        LOCAL        UNKNOWN                                  */
/*  TTY                                    APPEND                                   */
/*  TTYM                                   RENEW                                    */
/*  LOG                                                                             */
/*  BRKSHRT                                                                         */
/*  SYSEXC                                                                          */
/*  MARPAD                                                                          */
/*                                                                                  */
/*  The following options are not mutually exclusive but will generally only apply  */
/*  to files.                                                                       */
/*                                                                                  */
/*  MIRROR    (Generally only used with files - MIRROR command must be specified)   */
/*  TIE  lu   (Associates current lu with the specified lu)                         */
/*  RECL n    (Defines the associated FORTRAN Record Length)                        */
/*                                                                                  */
/*==================================================================================*/

static char   * assign_options[] = {

                         "CON",             /* CONSOLE Device                       */
                         "PR",              /* PRINTER Device                       */
                         "CO",              /* CONTIGUOUS File (256 byte sectors)   */
                         "IN",              /* INDEX File (Variable Record Lengths) */
                         "BRKSHRT",         /* BRKSHRT Terminal Assignment          */
                         "SYSEXC",          /* SYSEXC Terminal Assignment           */
                         "MARPAD",          /* MARPAD Terminal Assignment           */
                         "RANDOM",          /* Allow Random I/O                     */
                         "SEQUENTIALLY",    /* Allow Sequential I/O                 */
                         "TTY",             /* TTY Device                           */
                         "LOG",             /* LOG (Logging) Device                 */
                         "CLIENT",          /* Client Side of a TCP/IP connection   */
                         "SERVER",          /* Server Side of a TCP/IP connection   */
                         "TCP",             /* Server Side of a TCP/IP connection   */
                         "LOCAL",           /* Local Device Assignment              */
                         "RECL",            /* Record Length of File                */
                         "TIE",             /* Link an LU to another LU             */
                         "TTYM",            /* Special TTY Assignment               */
                         "MIRROR",          /* Mirror I/O to Mirror File            */

                         "OLD",             /* File must exist                      */
                         "NEW",             /* File must not currently exist        */
                         "SCRATCH",         /* File is a scratch file               */
                         "UNKNOWN",         /* File Status is not known             */
                         "APPEND",          /* Append I/O to the file               */
                         "SEQUENTIAL",      /* Sequential I/O to the file           */
                         "DIRECT",          /* Direct (Random) I/O to the file      */
                         "FORMATTED",       /* The file is formatted                */
                         "UNFORMATTED",     /* The file is unformatted (binary)     */
                         "PRINT",           /* The file is print file               */
                         "READONLY",        /* The file is open'ed readonly         */
                         "RENEW",           /* The file is to reallocated if exist  */
                         (char *)NULL 
                        } ;


/*==================================================================================*/
/* Get Keyword (Command)                                                            */
/*==================================================================================*/

static int      GetKeyWord(unsigned char * buffer, int      * index, 
                           int             length)
{
  int               offset;               /* Current offset into buffer             */
  int               endcmd;               /* End of the command                     */
  int               status;               /* Status Variable                        */
  int               cmdlen;               /* Length of the command                  */
  int               loopcnt;              /* Loop counter                           */
  unsigned char     cmd[CMDSIZE];         /* Command work buffer                    */

  offset = fnblnk(buffer,(*index),length);/* Move to the Command/Logical Unit       */
  if(offset <  length) {                  /* We have a command                      */
    (*index) = offset;                    /* Update index                           */
    endcmd = fblnk(buffer,offset,length); /* Find the end of the command            */
    cmdlen = endcmd - offset;             /* Compute length of the command          */
    memset(cmd,0x00,CMDSIZE);             /* Clear the work buffer                  */
    memcpy(cmd,&(buffer[offset]),cmdlen); /* Move the command for checking          */
    cnvupper(cmd);                        /* Insure the command is uppercase        */
    for(loopcnt=0;;loopcnt++) {           /* Check each possible command            */
      if(cmnds[loopcnt]==NULL) {          /* At the end of the table - check for lu */
        status = alldigits(cmd);          /* Check for all numbers                  */
        break;                            /*                                        */
      }                                   /*                                        */
      status = strcmp((char *)cmd,cmnds[loopcnt]);/* Do the two strings match               */
      if(status == 0L) {                  /* The strings match - so set the status  */
        status   = loopcnt;               /* Set the status                         */
        (*index) = endcmd;                /* Point to byte after command            */
        break;                            /*                                        */
      }                                   /*                                        */
    }                                     /*                                        */
  }                                       /*                                        */
  else {                                  /* No command was found                   */
    status = -2L;                         /*                                        */
  }                                       /*                                        */
  return(status);                         /* Return the status to the caller        */
}                                         /*                                        */

//static int      getkeyword_(unsigned char * buffer, int      * index,
//                            int           * length)
//{
//  int         status;                     /* Status to return to the caller         */
//
//  status = GetKeyWord(buffer,index,(*length));
//  return(status);
//}


/*==================================================================================*/
/* Get String (Options, Arguments)                                                  */
/*==================================================================================*/

int      GetOptions(unsigned char * buffer, int           * index,
                           int             length, unsigned char * option)
{
  int               offset;               /* Current offset into buffer             */
  int               endopt;               /* End of the command                     */
  int               status;               /* Status Variable                        */
  int               optlen;               /* Length of the command                  */
  unsigned char     opt[OPTSIZE];         /* Option work buffer                     */

  offset = fnblnk(buffer,(*index),length);/* Move to the Command/Logical Unit       */
  if(offset <  length) {                  /* We have a command                      */
    (*index) = offset;                    /* Update index                           */
    endopt = fblnk(buffer,offset,length); /* Find the end of the command            */
    optlen = endopt - offset;             /* Compute length of the command          */
    memset(opt,0x00,OPTSIZE);             /* Clear the work buffer                  */
    memcpy(opt,&(buffer[offset]),optlen); /* Move the command for checking          */
    cnvupper(opt);                        /* Insure the command is uppercase        */
    strcpy((char *)option,(char *)opt);                   /* Move option to the caller              */
    status = endopt;                      /* Point after the option/argument        */
  }                                       /*                                        */
  else {                                  /* No command was found                   */
    status = -2L;                         /*                                        */
  }                                       /*                                        */
  return(status);                         /* Return the status to the caller        */
}                                         /*                                        */

//static int      getoptions_(unsigned char * buffer, int            * index,
//                            int           * length,  unsigned char * option,
//                            int             optlen)
//{
//  int               status;               /* Status to return to the caller         */
//  int               optsize;              /* Size of the option string              */
//  unsigned char     opt[OPTSIZE];         /* Option work buffer                     */
//
//  status = GetOptions(buffer,index,(*length),opt);
//  if(status == 0) {
//    memset(option,(int     ) ' ',optlen);
//    optsize = strlen((char *)opt);
//    optsize = ((optsize < optlen) ? optsize : optlen);
//    memcpy(option,opt,optsize);
//  }
//  return(status);
//}


/*==================================================================================*/
/* Get String (Options, Arguments)                                                  */
/*==================================================================================*/

static int      GetOptionsASIS(unsigned char * buffer, int           * index,
                               int             length, unsigned char * option)
{
  int               offset;               /* Current offset into buffer             */
  int               endopt;               /* End of the command                     */
  int               status;               /* Status Variable                        */
  int               optlen;               /* Length of the command                  */
  unsigned char     opt[OPTSIZE];         /* Option work buffer                     */

  offset = fnblnk(buffer,(*index),length);/* Move to the Command/Logical Unit       */
  if(offset <  length) {                  /* We have a command                      */
    (*index) = offset;                    /* Update index                           */
    endopt = fblnk(buffer,offset,length); /* Find the end of the command            */
    optlen = endopt - offset;             /* Compute length of the command          */
    memset(opt,0x00,OPTSIZE);             /* Clear the work buffer                  */
    memcpy(opt,&(buffer[offset]),optlen); /* Move the command for checking          */
    strcpy((char *)option,(char *)opt);                   /* Move option to the caller              */
    status = endopt;                      /* Point after the option/argument        */
  }                                       /*                                        */
  else {                                  /* No command was found                   */
    status = -2L;                         /*                                        */
  }                                       /*                                        */
  return(status);                         /* Return the status to the caller        */
}                                         /*                                        */

//static int      getoptionsasis_(unsigned char * buffer, int            * index,
//                                int           * length,  unsigned char * option,
//                                int             optlen)
//{
//  int               status;               /* Status to return to the caller         */
//  int               optsize;              /* Size of the option string              */
//  unsigned char     opt[OPTSIZE];         /* Option work buffer                     */
//
//  status = GetOptionsASIS(buffer,index,(*length),opt);
//  if(status == 0) {
//    memset(option,(int     ) ' ',optlen);
//    optsize = strlen((char *)opt);
//    optsize = ((optsize < optlen) ? optsize : optlen);
//    memcpy(option,opt,optsize);
//  }
//  return(status);
//}


/*==================================================================================*/
/* Option_Assign()                                                                  */
/*==================================================================================*/

static int      Option_Assign(unsigned char * option)
{
  int           status;
  int           wrkstatus;
  int           index;

  status    = -1L;
  for(index =  0L; assign_options[index] != (char *)NULL; index++) {
    wrkstatus = strcmp((char *)option,(char*)assign_options[index]);
    if(wrkstatus == 0L) {
      status = index;
      break;
    }
  }
  return(status);
}

//static int      option_assign_(unsigned char * option, int      length)
//{
//  int               status;
//  int               index;
//  unsigned char     opt[OPTSIZE];         /* Option work buffer                     */
//
//  memset(opt,0x00,OPTSIZE);
//  for(index=0;index<length;index++) {
//    if(index >= (OPTSIZE-1L)) {
//      break;
//    }
//    if(option[index] == ' ') {
//      break;
//    }
//    opt[index] = option[index];
//  }
//  status = Option_Assign(opt);
//  return(status);
//}

/*==================================================================================*/
/* Convert Command to Uppercase                                                     */
/*==================================================================================*/

static void cnvupper(unsigned char * buffer)
{
  int                byte;                       /* Byte to be checked              */
  int                index;                      /* Working Index                   */

  for(index=0;buffer[index]!='\0';index++) {     /* Check all bytes of the command  */
    byte          = (int     ) buffer[index];    /* Extract the byte to convert     */
    byte          = toupper(byte);               /* Convert the byte to uppercase   */
    buffer[index] = (unsigned char) byte;        /* Put the byte back               */
  }                                              /*                                 */
  return;                                        /* Return to the caller            */
}                                                /*                                 */

/*==================================================================================*/
/* Check if command is an Logical Unit                                              */
/*==================================================================================*/

static int      alldigits(unsigned char * buffer)
{
  int                rtnstatus;                  /* Status to return to caller      */
  int                byte;                       /* Byte to be checked              */
  int                index;                      /* Working Index                   */

  rtnstatus   = 0L;                              /* Assume that it is all numbers   */
  for(index=0;buffer[index]!='\0';index++) {     /* Check all bytes of the command  */
    byte      = (int     ) buffer[index];        /* Extract the byte to check       */
    if( ! (isdigit(byte))) {                     /* Not a digit ?                   */
      rtnstatus = -1L;                           /* True, Set status to -1          */
      break;                                     /* Exit processing loop            */
    }                                            /*                                 */
  }                                              /*                                 */
  return(rtnstatus);                             /* Return to the caller            */
}                                                /*                                 */

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
