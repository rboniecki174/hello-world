/*****************************************************************************/
/*                                                                           */
/* ENABLE routines: init(), enable() and other general support     R03-02.02 */
/*                                                                           */
/*  20 Jan 97    0.0   RAS    Initial implementation, in C                   */
/*  20 Feb 97    0.0   Pz     General rearrangement:                         */
/*                              queue_handler => enable_handler[7]           */
/*                              local_handler => queue_handler               */
/*                              trap 5        => trap 2                      */
/*                                       new  => trap 4                      */
/*                              MSGSIZE       => PARMSIZE = 4                */
/*                                       new  => MSGSIZE = 64                */
/*  21 Feb 97    0.1   RAS    Added trap 5 support (requires                 */
/*                            set_one_shot.c in os32lib.a)                   */
/*   2 Apr 97          Pz     Allow access to enable_handler[]               */
/*  17 Apr 97          Pz     Incorporate init() for enable() only           */
/*  15 Jul 1997  1.0   DRLJr  Overhauled code here and in set_one_shot().    */
/*  28 Aug 1997  1.1   DRLJr  Add Code for DISABL(0) to allow it to work     */
/*  18 Dec 1997  2.0   DRLJr  Added Code to handle termination requests and  */
/*                            to handle various types of signals for special */
/*                            processing that may be desired.                */
/*  05 Jan 1998  2.1   DRLJr  Changed the SIGPIPE signal trap from being     */
/*                            ignored to terminating the program with an End */
/*                            of Task code of 252.                           */
/*  11 Jan 1998  2.1.1 DRLJr  Added the routine settaskname_ to allow for    */
/*                            tasknames in messages without the need to use  */
/*                            the real-time common.  Changed the %-8.8s      */
/*                            format string to %-s in the various messages.  */
/*  03 Feb 1998  2.1.2 DRLJr  Break signal support into a seperate function. */
/*  04 Feb 1998  3.0.0 DRLJr  Added additional signal trap support and the   */
/*                            ability to generate a core file under certain  */
/*                            signal conditions.                             */
/*  12 Feb 1998  3.0.1 DRLJr  Insure that certain signal are not blocked     */
/*                            when in the "trap" state.  Added a special     */
/*                            gb_exit_() routine to allow for program        */
/*                            termination and clean-up.                      */
/*  13 Feb 1998  3.0.2 DRLJr  Further clean-up and switch from sleep() to    */
/*                            use the ANSI-C pause() function for enable(0). */
/*                            The pause() routine is awaken via a signal.    */
/*  17 Mar 1998  3.0.3 DRLJr  Added a gb_exit() routine for "C" that accepts */
/*                            the eot code by value.  gbexit_() accepts eot  */
/*                            code by address.                               */
/*  13 Apr 1998  3.0.4 DRLJr  Changed from the use of pause() in the code    */
/*                            used to support enable_(0) to the use of the   */
/*                            select() function.  select() does not make use */
/*                            of signal thus eliminating potential conflicts */
/*                            with the use of SIGALRM.                       */
/*  22 Apr 1998  3.0.5 DRLJr  Added code in the task queue handler to check  */
/*                            for potential error conditions.                */
/*  28 May 1998  3.0.6 DRLJr  Added missuing code to set the value of the    */
/*                            routine vector address for testing.            */
/*  27 Jul 1998  3.1.0 DRLJr  Added emulation of a atexit processing as per  */
/*                            Rich B. needs.  Atexit routines are invoked    */
/*                            via the common exit routine.  Routines are     */
/*                            invoked in the same order as the standard      */
/*                            "C" atexit routine.                            */
/*  27 Apr 1999  3.1.1 DRLJr  Added additional error message for errors with */
/*                            send_trap routine to aid in resolving resource */
/*                            errors with the message queues.                */
/*  28 Apr 1999  3.2.0 DRLJr  Added code to allow the processing of sigpipe  */
/*                            signals to either terminate the program or to  */
/*                            be ignored.                                    */
/*  01 Dec 1999  3.2.1 DRLJr  Fix code so that settaskname_() works correctly*/
/*  23 May 2000  3.2.2 DRLJr  Add a kill(pid,0) call to check if task is     */
/*                            still in memory prior to adding data to the    */
/*                            message queue.  Add code to prevent sending    */
/*                            more than maximum amount of data in msgque.    */
/*  09 Feb 2001 4.0.0 jeffsun Sun port. Remove unsignted from timer_id       */
/*                            declaration. Add sigdefs.h for signal values   */
/*                            Fix signal names, etc                          */
/*  03 Apr 2001       jeffsun Made warning for handler address for dgux only */
/*  03 Apr 2001       jeffs   Add sigprocmask to block and unblock in routine*/
/*                            queue_handler.  Not needed for the DG          */
/*  07/19/2007        jl      SCNTRADE                                       */
//  05/14/2014        gj      Clean up the prototypes
/*****************************************************************************/

#define  USE_NEW_CODE  1
#ifdef   USE_NEW_CODE
#ifdef dgux
#define  _POSIX4_DRAFT_SOURCE 199309
#endif  /* if dgux */
#endif

#define  BLOCK_YES     1
#define  BLOCK_NO      0
#define  YES_STACKDUMP     1
#define  NO_STACKDUMP      0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>   //for select fd_set def
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>

#include "proto_common.h" //tt80093

#if defined( __linux__)
  #include <bits/sigstack.h>
  int snprintf(char *str, size_t size, const char *format, ...);
#else 
  #include <siginfo.h>
#endif
#include <sys/ucontext.h>
#include <dlogdefs.h>
//#include "libos32.h" tt80093

#if defined(__sparc) || defined(__i386)
/*****************************************************************/
/* Note, that SIGDGTIMER4 is redefined on the SUN in sigdefs.h   */
/* along with other proprietary signals used.                    */
/*****************************************************************/
#include <sigdefs.h>                                      /*jeffsun-02/08/01*/
#endif /* __sparc */



/*===========================================================================*/
/* Signal Handlers and Other Prototypes                                      */
/*===========================================================================*/

void set_signals_();
void queue_handler();
void timer_handler();
void dgtimer4();
void dgtimer3();
void dgtimer2();
void dgtimer1();
void dgnotify();
void exit_hangup();
void exit_quit();
void exit_interrupt();
void exit_common();
void usersignal1();
void usersignal2();
void usersignal2x();
void sigpipe();
void sigalarm();
void sigterm();
void sigsegv();
void sigabrt();
void sigill();
void sigfpe();
void sigbus();

//void gb_atexit(void);                //tt80093
//void gb_exit_(int      * eot);       //tt80093
//void gb_exit(int      eot);          //tt80093
//void gb_addexit_(void (*handler)()); //tt80093
//void gb_addexit(void (*handler)());  //tt80093

/* Arms timer and returns timer_id for subsequent deletion */
//out unsigned int  set_one_shot(); 

void core_mvtname(char * tname);

int (*syshandler)();

extern void               write_eot(char * taskname);
extern void               set_mqid_(int      * mqid);
char *trim(char *s);
int  get_pid_(int      * ftnslot);
int  get_mid_(int      * ftnslot);
int  get_slot_(char * name, int      name_len);
int  get_msgid_(void);
int  mq_dele_(int      * mid);
int  clear_slot_(int      * slot_to_clear);
int  mq_init_(char * taskname,int * msg_key, int length);


/*===========================================================================*/
/* Signal Handlers Support Definitions and other definitions                 */
/*===========================================================================*/

#define WRTMSG(code) dLog(DLOG_MAJOR,sigfmt,mytaskname,signal_name[signal_code], \
                                                      signal_code,caugign[code])
#ifdef USE_NEW_CODE

#define SETSIGNAL(flags)                                                         \
  (void) sigemptyset(&signal_traps.sa_mask);                                     \
  signal_traps.sa_flags = (flags);                                               

#define BLKSIGNAL(numb)                                                          \
  (void) sigaddset(&signal_traps.sa_mask,numb);

#define ADDSIGNAL(numb,rtn)                                                      \
  signal_traps.sa_handler   = rtn;               /* Handler address   */         \
/*signal_traps.sa_sigaction = rtn;  */           /* Handler address   */         \
  status                    = sigaction(numb, &signal_traps,                     \
                                       (struct sigaction *) NULL);               \
  if(status != 0) {                                                              \
    status = errno;                                                              \
    errtxt = strerror(status);                                                   \
    dLog(DLOG_MAJOR,"Error setting up signal trap %s -- errno = %d (%s)",        \
            signal_name[numb],status,errtxt);                                    \
  }                                                                              \
  else {                                                                         \
  }                                                                              

#else

#define SETSIGNAL(flags)    /* Not used */
#define BLKSIGNAL(numb)     /* Not used */
#define ADDSIGNAL(numb,rtn) syshandler = signal(numb,rtn)

#endif

#define SIGNAL_IGNORED        0
#define SIGNAL_CAUGHT         1

#define ENABLE_TABLE_SIZE    10
#define UNXMSG_TABLE_SIZE    10

#define EXIT_TABLE_SIZE      64

#define PARMSIZE              4
#define MSGSIZE              72

#define NAMESIZE             48

#define MSGQUEUE_SIZE      1024

//void (*sigset (int sig, void (*disp)(int)))(int);    //tt80093 75914


/*===========================================================================*/
/* Signal Handlers Support Data and other data                               */
/*===========================================================================*/

extern           int      ECHO_QUEUE_PARMS; /* Located in queue.c */
static           int      init_inited       = {  0L };
static           int      init_eotcode      = {  0L };
static           int      exit_rtns         = {  0L };
char logFileName[128];        //75914 this is a cfg global 

void (*enable_handler[ENABLE_TABLE_SIZE])();  /* for enable() case's */
void (*unxmsg_handler[UNXMSG_TABLE_SIZE])();  /* for other handlers  */
void (*exit_routines[EXIT_TABLE_SIZE])();     /* for other handlers  */

#ifdef USE_NEW_CODE
static  struct sigaction  signal_traps;
//static  struct sigaction  signal_traps_block;
//static  struct sigaction  signal_traps_noblock;
#endif

/* Enable(0)/Disabl(0) Control Flag words                  */

static int       enadis         = { 1 } ;
static int       ignore_sigpipe = { 1 } ;  /* != 0 ==> terminate  */

static  char mytaskname[NAMESIZE+4] = 
                  { '!', 'n', 'o', 't', '-', 's', 'e', 't', '\0','\0','\0','\0',
                    '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',  
                    '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',  
                    '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
                    '\0','\0','\0','\0' };

static  char   sigfmt[]  = { " %-.8s : %-12.12s (%3d) signal received.  %s\n" };
static  char * caugign[] = { "(Ignored)", "" };

#if defined(__sparc) || defined(__i386)
static  char * signal_name[] = { 
 "SIGNULL",    "SIGHUP",     "SIGINT",     "SIGQUIT",    "SIGILL",      "SIGTRACE",
 "SIGABRT/IOT","SIGEMT",     "SIGFPE",     "SIGKILL",    "SIGBUS",      "SIGSEGV",
 "SIGSYS",     "SIGPIPE",    "SIGALRM",    "SIGTERM",    "SIGUSR1",     "SIGUSR2",
 "SIGCLD",     "SIGPWR",     "SIGWINCH",   "SIGURG",     "SIGPOLL/IO",  "SIGSTOP",
 "SIGTSTP",    "SIGCONT",    "SIGTTIN",    "SIGTTOU",    "SIGVTALRM",   "SIGPROF", 
 "SIGXCPU",    "SIGXFSZ",    "SIG-32",     "SIG-33",     "SIG-34",      "SIG-35",
 "SIG-36",     "SIG-37",     "SIGRTMIN38", "SIGRT39",    "SIGRT40",     "SIGRT41",
 "SIGRT42",    "SIGRT43",    "SIGRT44",    "SIGRTMAX45" };    /*jeffsun-02/08/01*/
#endif /* __sparc */

#ifdef dgux
static  char * signal_name[] = { 
 "SIGNULL",    "SIGHUP",     "SIGINT",     "SIGQUIT",    "SIGILL",      "SIGTRACE",
 "SIGABRT/IOT","SIGEMT",     "SIGFPE",     "SIGKILL",    "SIGBUS",      "SIGSEGV",
 "SIGSYS",     "SIGPIPE",    "SIGALRM",    "SIGTERM",    "SIGUSR1",     "SIGUSR2",
 "SIGCLD",     "SIGPWR",     "SIGWINCH",   "SIGURG",     "SIGPOLL/IO",  "SIGSTOP",
 "SIGTSTP",    "SIGCONT",    "SIGTTIN",    "SIGTTOU",    "SIGVTALRM",   "SIGPROF", 
 "SIGXCPU",    "SIGXFSZ",    "SIG-32",     "SIG-33",     "SIG-34",      "SIG-35",
 "SIG-36",     "SIG-37",     "SIG-38",     "SIG-39",     "SIGLOST",     "SIG-41",
 "SIG-42",     "SIG-43",     "SIG-44",     "SIG-45",     "SIG-46",      "SIG-47",
 "SIG-48",     "SIG-49",     "SIG-50",     "SIG-51",     "SIG-52",      "SIG-53",
 "SIG-54",     "SIG-55",     "SIG-56",     "SIG-57",     "SIG-58",      "SIG-59",
 "SIGDGTIMER1","SIGDGTIMER2","SIGDGTIMER3","SIGDGTIMER4","SIGDGNOTIFY", "SIG-65",
 "SIG-66",     "SIG-67",     "SIG-68",     "SIG-69",     "SIG-70",      "SIG-71",
 "SIG-72",     "SIG-73",     "SIG-74",     "SIG-75",     "SIG-76",      "SIG-77",
 "SIG-78",     "SIG-79",     "SIG-80",     "SIG-81",     "SIG-82",      "SIG-83",
 "SIG-84",     "SIG-85",     "SIG-86",     "SIG-87",     "SIG-88",      "SIG-89",
 "SIG-90",     "SIG-91",     "SIG-92",     "SIG-93",     "SIG-94",      "SIG-95",
 "SIG-96",     "SIG-97",     "SIG-98",     "SIG-99",     "SIG-100",     "SIG-101"
 "SIG-102",    "SIG-103",    "SIG-104",    "SIG-105"     "SIG-106"      "SIG-107",
 "SIG-108",    "SIG-109",    "SIG-110",    "SIG-111"     "SIG-112"      "SIG-113",
 "SIG-114",    "SIG-115",    "SIG-116",    "SIG-117"     "SIG-118"      "SIG-119",
 "SIG-120",    "SIG-121",    "SIG-122",    "SIG-123"     "SIG-124"      "SIG-125",
 "SIG-126",    "SIG-127" };
#endif /* dgux */


/*===========================================================================*/
/* Subroutine INIT() Function                                                */
/*                                                                           */
/*   Initialize the FORTRAN vector tables and set-up to handle the vector    */
/* traps.                                                                    */
/*                                                                           */
/*===========================================================================*/

void init_()
{
  int        i;
  int        status;
  int        key;
  char       taskname[12];
  char     * errtxt;

  if(init_inited != 0) {                                /* If already inited */
    return;                                             /* just return to    */
  }                                                     /* the caller        */

  for (i = 0; i < ENABLE_TABLE_SIZE; i++) {
    enable_handler[i] = 0;
  }
  for (i = 0; i < UNXMSG_TABLE_SIZE; i++) {
    unxmsg_handler[i] = 0;
  }
  enadis = 1;

  (void) get_tname_(taskname,8L);                       /* Get our task name */
  if(taskname[0] != '?') {
    memcpy(mytaskname,taskname,8L);
  }

  key    = 0L;
  status = mq_init_(mytaskname,&key,8L);
  if(status > 0L) {
    set_mqid_(&status);
  }

/*Set_Signals_();*/                                     /* Setup signals     */

  SETSIGNAL((SA_RESTART | SA_SIGINFO));                 /* Perform Set-up    */

  BLKSIGNAL(SIGUSR1);                                   /* Block SIGUSR1     */
  ADDSIGNAL(SIGUSR1,queue_handler);                     /* Handle SIGUSR1    */

  SETSIGNAL((SA_RESTART | SA_SIGINFO));                 /* Perform Set-up    */
  BLKSIGNAL(SIGUSR1);                                   /* Block SIGUSR1     */
/*****************************************************************/
/* Note, that SIGDGTIMER4 is redefined on the SUN in sigdefs.h   */
/* along with other proprietary signals used.                    */
/*****************************************************************/
//  BLKSIGNAL(SIGDGTIMER4);                               /* BLock SIGDGTIMER4 */
//  ADDSIGNAL(SIGDGTIMER4,timer_handler);                 /* Handle DGTIMER4   */

  init_inited = -1L;                                    /* Indicate setup    */

  return;

}



/*===========================================================================*/
/* Support Function SetSignals - Install default handlers for signals        */
/*===========================================================================*/

void set_signals_()
{
  int        i;
  int        status;
  char       taskname[12];
  char     * errtxt;

  /*-------------------------------------------------------------------------*/
  /* Get the taskname lf the task being run                                  */
  /*-------------------------------------------------------------------------*/

  (void) get_tname_(taskname,8L);                       /* Get our task name */
  if(taskname[0] != '?') {
    memcpy(mytaskname,taskname,8L);
  }

  /*-------------------------------------------------------------------------*/
  /* Set the taskname for the core generation support routine                */
  /*-------------------------------------------------------------------------*/

  core_mvtname(mytaskname);

  /*-------------------------------------------------------------------------*/
  /* Set-up the exit termination support code                                */
  /*-------------------------------------------------------------------------*/

  if(init_eotcode == 0L) {
    i = atexit(&gb_atexit);
    if(i != 0) {
      dLog(DLOG_MAJOR," %-s : Exit Registration of gb_atexit() failed. (%d)",
                             mytaskname,i);
    }
    else {
      dLog(DLOG_STDLOG," %-s : Exit Registration of gb_atexit() complete. (%d)",
                             mytaskname,i);
      init_eotcode = -1L;
    }
  }



  /*-------------------------------------------------------------------------*/
  /* Set-up the default signal environment                                   */
  /*-------------------------------------------------------------------------*/

  SETSIGNAL((SA_RESTART | SA_SIGINFO));                 /* Perform Set-up    */

  ADDSIGNAL(SIGUSR1,usersignal1);
  ADDSIGNAL(SIGUSR2,usersignal2);
  ADDSIGNAL(SIGDGTIMER1,dgtimer1);
/*
  ADDSIGNAL(SIGDGTIMER2,dgtimer2);
*/
  ADDSIGNAL(SIGDGTIMER3,dgtimer3);
  ADDSIGNAL(SIGDGTIMER4,dgtimer4);
  ADDSIGNAL(SIGDGNOTIFY,dgnotify);
  ADDSIGNAL(SIGHUP,exit_hangup);
  ADDSIGNAL(SIGQUIT,exit_quit);
  ADDSIGNAL(SIGINT,exit_interrupt);
  ADDSIGNAL(SIGALRM,sigalarm);
  ADDSIGNAL(SIGTERM,sigterm);
  ADDSIGNAL(SIGSEGV,sigsegv);
  if (strncmp(taskname,"SCNTRA", 6L) != 0) {
     ADDSIGNAL(SIGPIPE,sigpipe);
  }
  else {
     //see tcpio.c jl 20070719
  }
  ADDSIGNAL(SIGABRT,sigabrt);
  ADDSIGNAL(SIGILL,sigill);

  return;
}



/*===========================================================================*/
/* Subroutine SETTASKNAME function                                           */
/*===========================================================================*/

void settaskname_(char * name, int      * length)
{
  int        wlength;

  if((*length) > NAMESIZE) {
    wlength = NAMESIZE;
  }
  else {
    wlength = (*length);
  }

  if(mytaskname[0] == '!') {
    (void) memcpy(mytaskname,name,wlength);
  }

  /*-------------------------------------------------------------------------*/
  /* Set the taskname for the core generation support routine                */
  /*-------------------------------------------------------------------------*/

  core_mvtname(mytaskname);

  return;
}



/*===========================================================================*/
/* Subroutine ENABLE() Function                                              */
/*===========================================================================*/

void enable_(const int * vector, void (*handler)())
{
    int trap;
    int status;

    if(init_inited == 0L) {                   /* Call the init_() routine    */
      (void) init_();                         /* Call it                     */
    }                                         /*                             */

    trap   = (*vector);                       /* Copy the vector to trap     */

    switch (trap) {                           /* Process Selection           */

    case 0:  /* Enter Trap Wait ............................................ */
             enadis = 1;
             while (enadis) {
             /*sleep(10000000);*/
             /*pause();*/
               status = select(1L,(fd_set *) NULL,(fd_set *) NULL,
                                  (fd_set *) NULL,(struct timeval *) NULL);
             }
             break;

    case 1:  /* Device Trap ................................................ */
    case 2:  /* Task Queue Parameter Trap .................................. */
    case 3:  /* Task Message Trap .......................................... */
    case 4:  /* I/O Proceed Completion ..................................... */

             enable_handler[trap] = handler;
             break;

    case 5:  /* Timer Termination .......................................... */

             enable_handler[trap] = handler;
             break;

    case 6:  /* Power Restoration Trap ..................................... */
             dLog(DLOG_WARNING," Power Restoration Trap is not supported.");
             break;

    default: /* All others and unsupported traps ........................... */
             dLog(DLOG_WARNING," Enable Code %3d is not supported/unknown.");
             break;

    } /* End of switch (trap) .............................................. */

    return;
}



/*===========================================================================*/
/* Subroutine DISABL() Function                                              */
/*===========================================================================*/

void disabl_(int      * vector)
{
    int trap;

    trap   = (*vector);                       /* Copy the vector to trap     */

    switch (trap) {                           /* Process Selection           */

    case 0:  /* Enter Trap Wait ............................................. */
             enadis = 0;
             break;

    case 1:  /* Device Trap ................................................ */
    case 2:  /* Task Queue Parameter Trap .................................. */
    case 3:  /* Task Message Trap .......................................... */
    case 4:  /* I/O Proceed Completion ..................................... */

             enable_handler[trap] = 0L;
             break;

    case 5:  /* Timer Termination .......................................... */

             enable_handler[trap] = 0L;
             break;

    case 6:  /* Power Restoration Trap ..................................... */
             dLog(DLOG_WARNING," Power Restoration Trap is not supported.");
             break;

    default: /* All others and unsupported traps ........................... */
             dLog(DLOG_WARNING," Disable Code %3d is not supported/unknown.");
             break;

    } /* End of switch (trap) .............................................. */

    return;
}



/*===========================================================================*/
/* General Purpose Function to send a trap to a process (Task)               */
/*===========================================================================*/

static char snd_err[]  = {" %-8.8s : send_trap : msgsnd error %d (%s).\n" };
static char msg_err[]  =
  {" %-8.8s : msgctl    : status=%3d, Bytes(Curr/Max)=%5d/%5d, #Msgs on the queue=%5d\n"};
static char msg_err1[] =
  {" %-8.8s :           : PID of last msg sent on the queue=%5d, PID of last received on the queue=%5d\n"};
static char msg_err2a[] =
  {" %-8.8s :           : Check for orphaned message queues (ipcs -qa, ipcrm -q msqid)\n," };
static char msg_err2[] =
  {" %-8.8s :           : Check if the maximum messages per queue and per system have been reached.\n," };
static char msg_err3[] =
  {" %-8.8s : Process pid %d is not present in the system but is in realtime table.\n" };
static char msg_err4[] =
  {" %-8.8s : Attempting to send more than %d (or < 1) bytes of data.  Amount to be sent is %d.\n" };

int      send_trap(int      rc, int      mid, int      pid, char * buffer,
                   int      size)
{
  int        status;
  int        errsta;
  int        data_size;
  char     * cerror;
  struct msqid_ds    qid_buf;               /* Get message queue info        */

  struct my_msg {                           /* Structure of data to send     */
    int      mtype;                         /* Designated item Control       */
    int      os32_rc;                       /* OS/32 Reason Code             */
    int      msg_data[MSGQUEUE_SIZE/4];     /* Room for 1K of data           */
  } my_msg;                                 /*                               */

  if((size<=0) || (size>MSGQUEUE_SIZE)) {   /* Problem with the size         */
    dLog(DLOG_WARNING,msg_err4,mytaskname,MSGQUEUE_SIZE,size); /* Write err msg */
    return(-3);                             /* Indicate signal error         */
  }                                         /*                               */

  status = kill(pid,0x0000);                /* Check if task is present ?    */
  if(status < 0) {                          /* It is not present             */
    dLog(DLOG_WARNING,"After testing of for validity of the PID");    /* Write an error message        */
    dLog(DLOG_WARNING,msg_err3,mytaskname,pid);/* Write an error message        */
    return(-2);                             /* Indicate signal error         */
  }                                         /*                               */

  my_msg.mtype   = 1;                       /* Indicate we are OS/32         */
  my_msg.os32_rc = (rc & 0xFF);             /* Set the Reason Code           */
  (void) memcpy(my_msg.msg_data,buffer,size);  /* Copy the data              */
  data_size      = size + 4;                /* Size we are adding            */
 
  status = msgsnd(mid,&my_msg,data_size,IPC_NOWAIT); /* Put the data on queue*/
  if(status < 0) {                          /* Unable to add data to queue   */
    errsta = errno;
    status = msgctl(mid,IPC_STAT,&qid_buf);
    cerror = strerror(errsta);
    (void) get_tname_(mytaskname,8L);  
    dLog(DLOG_WARNING,snd_err,mytaskname,errsta,cerror);
    if(status < 0) {
      status = errno;
    }
    dLog(DLOG_WARNING,msg_err,mytaskname,status,qid_buf.msg_cbytes,
                                             qid_buf.msg_qbytes,
                                             qid_buf.msg_qnum);
    dLog(DLOG_WARNING,msg_err1,mytaskname,qid_buf.msg_lspid,qid_buf.msg_lrpid);
    dLog(DLOG_WARNING,msg_err2,mytaskname);
    return(-1);                             /* Indicate message send         */
  }                                         /*                               */

  status = kill(pid,SIGUSR1);               /* Send a signal to the task     */
  if(status < 0) {                          /* Unable to trap the task       */
    dLog(DLOG_WARNING,"After sending signal");    /* Write an error message        */
    dLog(DLOG_WARNING,msg_err3,mytaskname,pid);    /* Write an error message        */
    dLog(DLOG_WARNING,msg_err2,mytaskname);    /* Write an error message        */
    dLog(DLOG_WARNING,msg_err2a,mytaskname);    /* Write an error message        */
    return(-2);                             /* Indicate signal error         */
  }                                         /*                               */

  return( 0);                               /* Return a good status          */
}                                           /*                               */



/*===========================================================================*/
/* Generate a trap to myself                                                 */
/*===========================================================================*/

int      self_trap(int      rc, char * buffer, int      size)
{
  int      status;
  int      slot;
  int      mid;
  int      pid;
  int      taskname[3];

  (void) get_tname_((char *)taskname,8L);   /* Get our task name             */
  slot = abs(get_slot_((char *)taskname,8L));  /* Get our slot number           */
  mid  = get_mid_(&slot);                   /* Get the queue id              */
  pid  = get_pid_(&slot);                   /* Get the process pid           */

  status = send_trap(rc,mid,pid,buffer,size);  /* Call the trap routine      */

  return(status);                           /* Return a good status          */
}                                           /*                               */



/*===========================================================================*/
/* Queue Handler for FORTRAN OS/32 Specific Traps                            */
/*===========================================================================*/

//static char recv_que[] = {" %-8.8s : QUEUE       Queued Item received, Item is %08X, %d (Pending=%d)"};
static char recv_msg[] = {" %-8.8s : SNDMSG      Message Received From %-8.8s"};
static char os_err[]   = {" %-8.8s : Trap Fncs : Unknown OS type code : %5d.\n" };
int    getmqcount(int mqid);  //60634

void queue_handler(int      signal_code, siginfo_t * info)
{
  int        msg_id;
  int        r_bytes;
  int        ftn_index;                    /* FORTRAN Enable Index           */
  int      * pntr;                         /* General Purpose Pointer        */
  char     * cpntr;
  char     * cerror;
  int        errsta;
  int        rtn_vector;
  int        mcnt;                         //60634 count of pending msgs on queue.

  struct my_msg {
    int      mtype;                        /* Designated item Control        */
    int      os32_rc;                      /* OS/32 Reason Code              */
    int  msg_data[MSGQUEUE_SIZE/4];        /* Room for 1K of data            */
  } my_msg;



#if 0
  fprintf(stderr," queue_handler : signal=%5d, info=%08X.\n",signal_code,info);
  fprintf(stderr,"                 signo =%5d, code=%8d, value=%08X.\n",
                  info->si_signo,info->si_code,info->si_errno
                /*info->si_value.sival_int*/);
#endif

  /* Ignore signals while we are in the handler; lost signals are OK since   */
  /* we will read the message queue until it is empty                        */

#ifndef USE_NEW_CODE
  sigignore(SIGUSR1);
#else
#if defined(__sparc) || defined(__i386)
   sigprocmask(SIG_BLOCK,&signal_traps.sa_mask,(sigset_t *)NULL); /*jeffs    */
#endif /*if sparc */
#endif /*else ifndef */

  /* Figure out which message queue to pull message off of for processing    */

  msg_id = get_msgid_();


  /* Read messages off queue until there are no more items on the queue      */

  for(;;) {                                /* Process until we are done      */

    mcnt = getmqcount(msg_id);             //ttt 60634
    r_bytes = msgrcv(msg_id,&my_msg,sizeof(struct my_msg),0,IPC_NOWAIT);

    if(r_bytes <= 0) {                     /* If queue empty then exit       */
      errsta = errno;
      if(errsta == ENOMSG) {               /* No messages                    */
        break;                             /* Exit now                       */
      }                                    /*                                */
      cerror = strerror(errsta);
      (void) get_tname_(mytaskname,8L);
      dLog(DLOG_WARNING," %-8.8s : QueueHndlr : msgrcv status %5d (%s).",
              mytaskname,errsta,cerror);
      break;                               /* processing loop                */
    }                                      /*                                */

    if(my_msg.mtype != 1) {
      (void) get_tname_(mytaskname,8L);
      dLog(DLOG_WARNING,os_err,mytaskname,my_msg.mtype);
      continue;
    }

    /* There is something on the queue; figure out what to do!               */

#if 0
    fprintf(stderr," QueueHndlr : rc %8X, Data %08X \n",my_msg.os32_rc,      
                                                        my_msg.msg_data[0]); 
#endif

    switch(my_msg.os32_rc) {               /* Process based on reason code   */
      case 0x00 :                          /* Trap Generating Device         */
                  ftn_index = 1;           /* Select FORTRAN Handler         */
                  pntr      = &my_msg.msg_data[0];
                  break;                   /* Exit switch statement          */
      case 0x01 :                          /* Queue Parameter                */
                  ftn_index = 2;           /* Select FORTRAN Handler         */
                  pntr      = &my_msg.msg_data[0];
                  break;                   /* Exit switch statement          */
      case 0x06 :                          /* Send Message                   */
                  ftn_index = 3;           /* Select FORTRAN Handler         */
                  pntr      = &my_msg.msg_data[0];
                  cpntr     = (char *) &my_msg.msg_data[0];
                  break;                   /* Exit switch statement          */
      case 0x08 :                          /* I/O Proceed Complete           */
                  ftn_index = 4;           /* Select FORTRAN Handler         */
                  pntr      = (int      *) my_msg.msg_data[0];
                  break;                   /* Exit switch statement          */
      case 0x09 :                          /* Timer Termination              */
                  ftn_index = 5;           /* Select FORTRAN Handler         */
                  pntr      = &my_msg.msg_data[0];
                  break;                   /* Exit switch statement          */
      default   :                          /* Unsupported reason code        */
                  ftn_index = -1;          /* Set to -1 for testing          */
                  pntr      = (int      *) -1L;
    }                                      /* End of the switch              */



    /* If the ftn_index >= 0 and the vector is set call the subroutine       */

    if((ftn_index >= 0) && (ftn_index < ENABLE_TABLE_SIZE)) { /* handle RC   */
      rtn_vector = (int     ) enable_handler[ftn_index];      /* 3.0.6       */
      if(enable_handler[ftn_index] != 0) { /* The handler is defined         */
        if(ECHO_QUEUE_PARMS != 0) {        /* Echo the parameter/report msg  */
          (void) get_tname_(mytaskname,8L);
          if(my_msg.os32_rc == 0x01) {
            dLog(DLOG_CALLQUEUE, " %-8.8s : QUEUE       Queued Item received, Item is %08X, %d (Pending=%d)",
                        mytaskname,
                        my_msg.msg_data[0], 
                        my_msg.msg_data[0], 
                        mcnt);
          }
          if(my_msg.os32_rc == 0x06) {
            dLog(DLOG_STDLOG,recv_msg,mytaskname,&my_msg.msg_data[0]);
          }
        }
        (void) (enable_handler[ftn_index])(pntr);
      }                                    /*                                */
    }                                      /*                                */
    else {  /* ************************* INTERNAL ERROR ******************** */
      dLog(DLOG_MAJOR,"*****************************************************");
      dLog(DLOG_MAJOR,"*****************************************************");
      dLog(DLOG_MAJOR,"*");
      dLog(DLOG_MAJOR,"* INTERNAL ERROR detected in enable.c (task %s)",
                     mytaskname);
      dLog(DLOG_MAJOR,"* Value of ftn_index is %d - Out of range 0 to %d.",
                     ftn_index,ENABLE_TABLE_SIZE-1);
      dLog(DLOG_MAJOR,"* Task trap is being ignored.");
      dLog(DLOG_MAJOR,"*");
      dLog(DLOG_MAJOR,"*****************************************************");
      dLog(DLOG_MAJOR,"*****************************************************");
    }
 
  }                                        /* End of the for(;;)             */

  /* Check if we have an error reported from the message queue read          */

  if(r_bytes < 0) {
    if(errno != ENOMSG) {
      perror(" Error occurred on Task Queue:  errno = ");
    }
  }

  /* All finished - Re-enable signal handler to allow for new traps          */
#ifndef USE_NEW_CODE
  syshandler = signal(SIGUSR1, queue_handler);
#else
#if defined(__sparc) || defined(__i386)    /* on Sun need to explictly block */
   sigprocmask(SIG_UNBLOCK,&signal_traps.sa_mask,(sigset_t *)NULL); /*jeffs    */
#endif /*ifdef sparc*/
#endif /*ifndef*/

  return;
}

//out
//out
//out/*===========================================================================*/
//out/* Special Timer Handler to add timer traps to the task queue                */
//out/*===========================================================================*/
//out
//outvoid timer_handler(int      signal_code, siginfo_t * info)
//out{
//out  int       status;
//out
//out#if 0
//out  fprintf(stderr," timer_handler : signal=%5d, info=%08X.\n",signal_code,info);
//out  fprintf(stderr,"                 signo =%5d, code=%8d, value=%08X.\n",
//out                  info->si_signo,info->si_code,info->si_errno
//out               /*info->si_value.sival_int*/);
//out#endif
//out
//out  /* timer has expired; call user routine, pass the parameter, delete timer  */
//out  /* timer canceled before calling the handler to avoid problem with         */
//out  /* recursion                                                      bs970326 */
//out
//out  status      = del_one_shot();
//out  if(status < 0L) {                              /* Timer is not active      */
//out  }                                              /*                          */
//out  else if(status == 0L) {                        /* Timer is gone            */
//out  }                                              /*                          */
//out  else {                                         /* Error in cancel          */
//out    dLog(DLOG_MAJOR," timer_handlr : Error deleting timer event!  Parameter=%8d.",
//out            param);
//out  }
//out
//out  if(enable_handler[5L] != 0L) {        
//out    status = self_trap(0x09, (char *) &param, 4L);
//out  }
//out
//out  return;
//out}
//out
//out
//out
//out/*===========================================================================*/
//out/* Cancel Existing Timer                                                     */
//out/*                                                                           */
//out/*   This FORTRAN callable routine will cancel a pending timer event.        */
//out/*                                                                           */
//out/*===========================================================================*/
//out
//outvoid timer_cancel_()
//out{
//out  int      status;
//out
//out  status   = can_one_shot();                     /* Cancel the timer         */
//out  if(status < 0L) {                              /* Timer is not active      */
//out  }                                              /*                          */
//out  else if(status == 0L) {                        /* Timer is gone            */
//out  }                                              /*                          */
//out  else {                                         /* Error in cancel          */
//out    dLog(DLOG_MAJOR," Timer_Cancel : Error canceling timer event!  Parameter=%8d.",
//out            param);
//out  }
//out
//out  return;
//out}
//out
//out
//out
//out /*===========================================================================*/
//out /* Invoke a one-shot timer event                                             */
//out /*                                                                           */
//out /* When called from Fortran, "one_shot" provides a clock which expires       */
//out /* "delay" milliseconds from now, and passes the parameter "arg2" to a       */
//out /* previously installed handler (installed via a "call enable(5,xyz) ".      */
//out /* If the user has NOT installed the handler, they can NOT call this guy     */
//out /*                                                                           */
//out /*===========================================================================*/
//out 
//out void one_shot_(int      * delay,int      * arg2)
//out {
//out   int          msecs;
//out   int          status;
//out 
//out   if(enable_handler[5L] == 0L) {
//out     dLog(DLOG_MAJOR," One-Shot: Attempting to load Timer, but no handler installed !");
//out     dLog(DLOG_MAJOR,"           You MUST first Call Enable(5,xxx) before using this !");
//out     return;
//out   /*exit(4);*/
//out   }
//out 
//out   msecs         =  (*delay);
//out   status        =  set_one_shot(msecs);
//out   if(status >= 0L) {
//out     timer_id    = status;
//out     param       = (*arg2);
//out   }
//out   else if(status == -1L) {
//out     dLog(DLOG_MAJOR," One_Shot : Unable to create timer event!  Parameter=%8d.",
//out           (*arg2));
//out   }
//out   else if(status == -2L) {
//out     dLog(DLOG_MAJOR," One_Shot : Unable to start timer event!  Parameter=%8d.",
//out             (*arg2));
//out   }
//out   else if(status == -3L) {
//out     dLog(DLOG_MAJOR," One_Shot : Attempt to reload Unexpired Timer!  Parameter=%8d.",
//out             (*arg2));
//out   }
//out   else {
//out     dLog(DLOG_MAJOR," One_Shot : Error started timer event, status=%d, Parameter=%8d.",
//out             status,(*arg2));
//out   }
//out 
//out   return;
//out }

void do_stackdump(void)
{
    int       pid = getpid();
    static char pstack[256]; 
    char out_filename[128];

#if defined( __linux__)
    //-----------------------------------------------------------
    // 75914 get the output file path from the configured logfile
    // create an output filename like "stackdump-DEPTHHDL-11234.log"
    //-----------------------------------------------------------
    if (logFileName[0] != '\0')
    {
      char *tmps = strrchr(logFileName, '/');    /* ptr to last occurance of '/'.  i.e.,"/production/log/" */
      int lendiff = strlen(logFileName)-strlen(tmps);
      sprintf(out_filename, "%.*s/stackdump-%s-%d.log", lendiff, logFileName, trim(mytaskname),pid);
    }
    else  {
      sprintf(out_filename, "stackdump-%s-%d.log", trim(mytaskname),pid);
    }
#else
    snprintf(out_filename,sizeof(out_filename),"core_stackdump.%s-%d", trim(mytaskname), pid);  
#endif

    // create a stack dump to be appended to the stdout log
    snprintf (pstack,sizeof(pstack), "/usr/bin/pstack %d", pid);                /* jhs 11/18/08 */
    system(pstack);                                              /* jhs 11/18/08 */
    //-------------------------------------------------------
    // 62413 DUMP the stack to a small file for analysis. 
    // Append the df output as evidence of frequent disk full status.
    //-------------------------------------------------------
    snprintf(pstack,sizeof(pstack), "date                > %s", out_filename);
    system(pstack);                                            
    snprintf(pstack,sizeof(pstack), "/usr/bin/pstack %d >> %s", pid, out_filename);
    system(pstack);                                            
    snprintf(pstack, sizeof(pstack), "/production/integrate/DM >> %s", out_filename);
    system(pstack);                                             
    snprintf(pstack, sizeof(pstack), "df -k             >> %s", out_filename);
    system(pstack);                                             
    snprintf(pstack,sizeof(pstack), "date                > %s", out_filename);
    dLog(DLOG_MAJOR, "FILE=%s",out_filename);
}

/*===========================================================================*/
/* Signal Interrupt Handlers for QUIT, INTERRUPT and others                  */
/*                                                                           */
/* This code is set-up to handle the following signals :                     */
/*                                                                           */
/*      SIGHUP      SIGINT     SIGQUIT     SIGUSR2       SIGPIPE             */
/*      SIGALRM     SIGTERM                                                  */
/*                                                                           */
/* The following signals are handled by routines above:                      */
/*                                                                           */
/*      SIGUSR2        - Task Queue Handler Interrupt Notification           */
/*      SIGDGTIMER4    - Pre-Timer Notification Interrupt Handler            */
/*                                                                           */
/* Note: Additional functions are supported as well as forcing the           */
/*       generation of a core file                                           */
/*                                                                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void exit_common(int      signal_code, siginfo_t * info, int stackdump_flag)   /* jhs 11/18/08 */
{
  int           mq_id;                        /* Message Queue to clear      */
  int           my_slot;                      /* Slot to be cleared          */
  int           status;                       /* Special purpose status      */
  int           index;                        /* General purpose index       */

  /* Note: Routines are invoked in the reverse order of registration so as   */
  /*       operate the same way at the "C" atexit() function.                */

  if (stackdump_flag)
  {                 
    //  folowing is for systems like sun/linux, supporting proc tools.
    //  Provide more information before exiting or crashing
    do_stackdump();                            //75914
  }

  if(exit_rtns > 0L) {                        /* Exit routines are registered*/
    for(index=exit_rtns-1;index>=0;index--) { /* Walk the list backwards     */
      (void) (exit_routines[index])();        /* Invoke the routine          */
    }                                         /*                             */
  }                                           /*                             */

  mq_id    = -1L;                             /* Delete the message queue we */
  status   = mq_dele_(&mq_id);                /* use to receive messages     */


  my_slot  =  0L;                             /* Clear our slot so we may    */
  status   = clear_slot_(&my_slot);           /* not be sent signals         */

  dLogFlushAll();                                              /* jhs 11/18/08 */
  write_eot(mytaskname);                      /* Write EOT message           */

  return;                                     /* Return to the caller        */
}



/*===========================================================================*/
/*                                                                           */
/* This routine is designed to be called via exit routines when a program    */
/* exits.  If a routine calls gb_exit_ (FORTRAN GB_EXIT) then this routine   */
/* may be invoked twice which is okay.  Currently (13/Feb/1998) the FORTRAN  */
/* EXIT routine will not handle the at_exit requests.                        */
/*                                                                           */
/*===========================================================================*/

void gb_exit_(int      * eot)
{
  gb_exit((*eot));
  return;
}

void gb_exit(int      eot)
{
  int         signal_code;
  siginfo_t   info;

  signal_code  = -1L;

  exit_common(signal_code,&info,NO_STACKDUMP);
  exit(eot);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void gb_addexit_(void (*handler)())
{
  gb_addexit(handler);
  return;
}

void gb_addexit(void (*handler)()) 
{
  if(exit_rtns < EXIT_TABLE_SIZE) {
    exit_routines[exit_rtns] = handler;
    exit_rtns++;
  }
  else {
    dLog(DLOG_MAJOR,"Unable to register exit routine - table full");
  }
  return;
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

/*                             0         1         2         3               */
/*                             0123456789012345678901234567890123456789      */
static char     corename[] = {"/bin/gcore -o core-xxxxxxxxxxxx ddddd  " };
#define         CORE_FNSTRT    19
#define         CORE_FNSTOP    30
#define         CORE_FNLENG    (CORE_FNSTOP - CORE_FNSTRT + 1)
#define         CORE_PIDSTRT   32
#define         CORE_PIDSTOP   36
#define         CORE_PIDLENG   (CORE_PIDSTOP - CORE_PIDSTRT + 1)
void call_gcore(int sig)
{
#if defined(__linux__)
  //-------------------------------------------------------------------
  //75419 Allow core to go to default location.  Do not use gcore.
  //-------------------------------------------------------------------
  sigset(sig, SIG_DFL); // set signal handler back to default    //75914
  kill(getpid(), sig);  // explicitly send this signal again      //75914
#else
  system(corename);
#endif
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void core_mvtname(char * tname)
{
  int          index;
  pid_t        pid;

  if(tname[0] != '?') {
    for(index=CORE_FNSTRT;index<CORE_FNSTOP+1;index++) {
      corename[index] = ' ';
    }
    for(index=0;index<CORE_FNLENG;index++) {
      if((tname[index] <= ' ') || (tname[index] > '~')) {
        break;
      }
      corename[index+CORE_FNSTRT] = tname[index];
    }
  }

  pid  = getpid();
  sprintf(&(corename[CORE_PIDSTRT]),"%5d",pid);

  return;
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void gb_atexit(void)
{
  int         signal_code;
  siginfo_t   info;

  signal_code  = -1L;

  exit_common(signal_code,&info,NO_STACKDUMP);
  return;
}
/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void exit_hangup(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void usersignal1(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void usersignal2(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void dgtimer1(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void dgtimer2(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void dgtimer3(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void dgtimer4(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void dgnotify(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigpipe(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_CAUGHT);
  if(ignore_sigpipe) {                   /* 0 means ignore */
    exit_common(signal_code,info,YES_STACKDUMP);
    exit(252);
  }
  return;
}

void set_sigpipe(int      flag)
{
  ignore_sigpipe = flag;
  return;
}

void set_sigpipe_(int      * flag)
{
  set_sigpipe((*flag));
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigalarm(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigterm(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_IGNORED);
  return;
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void exit_quit(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_CAUGHT);
  /* no stackdump since this is the CANCEL signal for normal exit */
  exit_common(signal_code,info,NO_STACKDUMP);
  exit(250);
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void exit_interrupt(int      signal_code, siginfo_t * info)
{
  WRTMSG(SIGNAL_CAUGHT);
  //exit_common(signal_code,info,YES_STACKDUMP);
  exit_common(signal_code,info,NO_STACKDUMP);
  exit(251);
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigsegv(int      signal_code, siginfo_t * info, ucontext_t * context)
{
//  int       i,j;
  FILE *fd;

  WRTMSG(SIGNAL_CAUGHT);
//  dLog(DLOG_MAJOR," A(info) = %08X signo=%8d, errno=%8d, code=%8d",
//         info,info->si_signo,info->si_errno,info->si_code);
//  dLog(DLOG_MAJOR," A(UCTX) = %08X flags=%08X, nxtctx=%8X",
//         context,context->uc_flags,context->uc_link);
//  j = 0;
  fd = dLogTypes[DLOG_MAJOR].fd;
//  for(i=0;i<NGREG;i++) {
//    fprintf(fd," Index:%5d, Register:%08X ",i,context->uc_mcontext.gregs[i]);
//    if(j >= 2) {
//      fprintf(fd,"\n");
//      j = 0;
//    }
//    else {
//      j++;
//    }
//  }
//  if(j != 0) {
//    fprintf(fd,"\n");
//  }
  exit_common(signal_code,info,YES_STACKDUMP);
  call_gcore(signal_code);
  exit(251);
}

/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigabrt(int      signal_code, siginfo_t * info)
{

  WRTMSG(SIGNAL_CAUGHT);
  dLog(DLOG_MAJOR,"A(info) = %08X signo=%8d, errno=%8d, code=%8d",
         info,info->si_signo,info->si_errno,info->si_code);
  exit_common(signal_code,info,YES_STACKDUMP);
  call_gcore(signal_code);
  exit(251);
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigill(int      signal_code, siginfo_t * info, ucontext_t * context)
{
  int       i,j;
  FILE *fd;

  WRTMSG(SIGNAL_CAUGHT);
  dLog(DLOG_MAJOR," A(info) = %08X signo=%8d, errno=%8d, code=%8d",
         info,info->si_signo,info->si_errno,info->si_code);
  dLog(DLOG_MAJOR," A(UCTX) = %08X flags=%08X, nxtctx=%8X",
         context,context->uc_flags,context->uc_link);
  j = 0;
  fd = dLogTypes[DLOG_MAJOR].fd;
  for(i=0;i<NGREG;i++) {
    fprintf(fd," Index:%5d, Register:%08X ",i,context->uc_mcontext.gregs[i]);
    if(j >= 2) {
      fprintf(fd,"\n");
      j = 0;
    }
    else {
      j++;
    }
  }
  if(j != 0) {
    fprintf(fd,"\n");
  }
  exit_common(signal_code,info,YES_STACKDUMP);
  call_gcore(signal_code);
  exit(251);
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigfpe(int      signal_code, siginfo_t * info, ucontext_t * context)
{
  int       i,j;
  FILE *fd;

  WRTMSG(SIGNAL_CAUGHT);
  dLog(DLOG_MAJOR," A(info) = %08X signo=%8d, errno=%8d, code=%8d",
         info,info->si_signo,info->si_errno,info->si_code);
  dLog(DLOG_MAJOR," A(UCTX) = %08X flags=%08X, nxtctx=%8X",
         context,context->uc_flags,context->uc_link);
  j = 0;
  fd = dLogTypes[DLOG_MAJOR].fd;
  for(i=0;i<NGREG;i++) {
    fprintf(fd," Index:%5d, Register:%08X ",i,context->uc_mcontext.gregs[i]);
    if(j >= 2) {
      fprintf(fd,"\n");
      j = 0;
    }
    else {
      j++;
    }
  }
  if(j != 0) {
    fprintf(fd,"\n");
  }
  exit_common(signal_code,info,YES_STACKDUMP);
  call_gcore(signal_code);
  exit(251);
}



/*===========================================================================*/
/*                                                                           */
/*===========================================================================*/

void sigbus(int      signal_code, siginfo_t * info, ucontext_t * context)
{
  int       i,j;
  FILE *fd;

  WRTMSG(SIGNAL_CAUGHT);
  dLog(DLOG_MAJOR," A(info) = %08X signo=%8d, errno=%8d, code=%8d",
         info,info->si_signo,info->si_errno,info->si_code);
  dLog(DLOG_MAJOR," A(UCTX) = %08X flags=%08X, nxtctx=%8X",
         context,context->uc_flags,context->uc_link);
  j = 0;
  fd = dLogTypes[DLOG_MAJOR].fd;
  for(i=0;i<NGREG;i++) {
    fprintf(fd," Index:%5d, Register:%08X ",i,context->uc_mcontext.gregs[i]);
    if(j >= 2) {
      fprintf(fd,"\n");
      j = 0;
    }
    else {
      j++;
    }
  }
  if(j != 0) {
    fprintf(fd,"\n");
  }
  exit_common(signal_code,info,YES_STACKDUMP);
  call_gcore(signal_code);
  exit(251);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

