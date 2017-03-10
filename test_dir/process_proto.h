/************************************************************************************/
/*                                                                                  */
/* process_proto.h     RUN-FILE support Prototype Statements              R00-00.00 */
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
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        05/Apr/1999  New file created for process_asn support    */
/*                                                                                  */
/************************************************************************************/

/*==================================================================================*/
/* process_asnc.c                                                                   */
/*==================================================================================*/

void              Process_Asn(unsigned char * BaseName, int      * status);

/*==================================================================================*/
/* process_asnf.c                                                                   */
/*==================================================================================*/

void              process_asn_(int      * status);

/*==================================================================================*/
/* process_asnc.c and process_asnf.c                                                */
/*==================================================================================*/

static int        AttachLU_Language(void);
static int        DettachLU_Language(int        lu);

/*==================================================================================*/
/* process_base.c                                                                   */
/*==================================================================================*/

static void       Process_Asn_Common(UCHAR * BaseName, int      * rtnstatus);

static int        Cmd_Task(  UCHAR * rdbuffer, int      * offset, int       length);
static int        Check_EOL( UCHAR * rdbuffer, int      * offset, int       length);
static int        Check_CMNT(UCHAR * rdbuffer, int        offset, int       length);

static int        Get_Debug_Option(UCHAR    * rdbuffer, int      * offset, 
                                   int        length);
static int        Get_Numeric(UCHAR * rdbuffer, int      * offset, int      length);


/*==================================================================================*/
/* process_assign.c                                                                 */
/*==================================================================================*/

static int        Cmd_GetPath(UCHAR * rdbuffer, int      * offset, int       length,
                              int      cmd,     UCHAR * pathbuff, int        pathlen);

static int        Call_RunFile(UCHAR * rdbuffer, int      * offset, int       length, 
                               int       cmd);

static int        Call_AsnFile(UCHAR * rdbuffer, int      * offset, int       length,
                               int       cmd);

static void       Set_Defaults(void);

static int        Get_ASNOption(UCHAR * rdbuffer, int      * offset, int       length,
                                UCHAR * option);

static void       Process_ASN_Clear_Common(void);

static int        AttachLU(void);
static int        AttachLU_TCP(void);
static int        AttachLU_LOCAL(void);

static int        PreFillFile(int      unixfd);

/*==================================================================================*/
/* process_runfiles.c                                                               */
/*==================================================================================*/

static    void MakeASNFileName( UCHAR    * BaseName,   UCHAR    * Extent,
                                UCHAR    * Buffer);
//static    void makeasnfilename_(UCHAR    * BaseName,   UCHAR    * Extent,
//                                UCHAR    * Buffer,     int        BaseNameLn,
//                                int        ExtentLn,   int        BufferLn);

static    int       OpenASNFile( FILE ** stream, UCHAR * File);
//static    int       openasnfile_(FILE ** stream, UCHAR * File, int      length);

static    int       ReadASNFile( FILE *  stream, UCHAR * Buffer, int      length);
//static    int       readasnfile_(FILE ** stream, UCHAR * Buffer, int      length);

static    int       CloseASNFile( FILE * stream);
//static    int       closeasnfile_(FILE ** stream);

static    int       AssignRunFile( UCHAR    * BaseName, FILE ** stream);
//static    int       assignrunfile_(UCHAR    * BaseName, FILE ** stream,
//                                  int        BaseNameLn);

/*==================================================================================*/
/* process_cmds.c                                                                   */
/*==================================================================================*/

static int      GetKeyWord(unsigned char * buffer, int      * index,
                           int             length);

//static int      getkeyword_(unsigned char * buffer, int      * index,
//                            int           * length);

static int      GetOptions(unsigned char * buffer, int           * index,
                       int             length, unsigned char * option);

//static int      getoptions_(unsigned char * buffer, int           * index,
//                            int           * length, unsigned char * option,
//                            int             optlen);

static int      GetOptionsASIS(unsigned char * buffer, int      *      index,
                               int             length, unsigned char * option);

//static int      getoptionsasis_(unsigned char * buffer, int      *      index,
//                                int           * length, unsigned char * option,
//                                int             optlen);

static int      Option_Assign(unsigned char * option);
//static int      option_assign_(unsigned char * option, int      length);

static void cnvupper(unsigned char * buffer);

static int      alldigits(unsigned char * buffer);

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
