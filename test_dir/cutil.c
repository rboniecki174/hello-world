/**********************************************************************/
/* cutil.c        Support Code for C/FORTRAN                          */
/*                                                                    */
/* Revisions:                                                         */
/*                                                                    */
/*  11/Dec/1997 DRLJr Corrected call arguements for the routine       */
/*                    mq_init_().  This routine is called from        */
/*                    FORTRAN to set-up the message queues.  ALL of   */
/*                    arguments are PASSED BY ADDRESS.  Corrected the */
/*                    argument list, included additional headers etc. */
/*                                                                    */
/*  12/Dec/1997 DRLJr Restricted reading of the created MSG Queue to  */
/*                    the owning task.                                */
/*                                                                    */
/*  05/Jan/1998 DRLJr Changed the message queue creation to private   */
/*                    instead of key based created.                   */
/*                                                                    */
/*  11/Jan/1998 DRLJr Changed queue delete logic that if the queue id */
/*                    is zero then do not try to delete the queue     */
/*                                                                    */
/*  21/Jun/2002 DRLJr Added additional error message text in mq_init  */
/*                    changed working on another error message        */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dlogdefs.h>

pid_t getpgid(pid_t pid);

#define  IPC_USE_PRIVATE    1    /* 0 = Create based on msg_key       */
                                 /* 1 = Create private message queue  */

#define  LCL_IPC_ACCESS  0622    /* U = Rd/Wr, G = Wr, O = Wr         */

#if (IPC_USE_PRIVATE == 0)
#define  LCL_IPC_KEY    (*msg_key)
#else
#define  LCL_IPC_KEY    (IPC_PRIVATE)
#endif
#define  LCL_IPC_FLAG   (LCL_IPC_ACCESS | IPC_CREAT)

int      get_msgid_();

int      mq_init_(char * taskname,int      * msg_key, int      length)
{
 int         m_id;
 int         status;
 int         index;
 int         tlength;
 char        ctaskname[12];
 char      * cerror;
 struct    {
  int        mtype;
  int        os32_rc;
  int        data[256];
 } msgdat;
 int         rcvlen;
 int         errsta;

 (void) memset(ctaskname,0L,12L);
 tlength = ((length <= 8) ? length : 8);
 for(index=0;index<tlength;index++) {
  ctaskname[index] = taskname[index];
 }

 status = msgget(LCL_IPC_KEY, LCL_IPC_FLAG );
 if(status == -1) {
   errsta = errno;
   cerror = strerror(errno);
   dLog(DLOG_MAJOR,"mq_init() : Error Creating Message Queue for %s.",
           ctaskname);
   dLog(DLOG_MAJOR,"          : Error Code %4d (%s)",errsta,cerror);
   dLog(DLOG_MAJOR,"          : Check for orphaned message queues with ipcs.");
   m_id = -1L;
 }
 else {
   m_id = status;
   /*---------------------------------------------------------------*/
   /* Remove all pending message queue data - start empty           */
   /*---------------------------------------------------------------*/
   for(;;) {
     rcvlen = msgrcv(m_id,&msgdat,sizeof(msgdat),0,IPC_NOWAIT);
     if(rcvlen <= 0) {
       if(errno != ENOMSG) {
         errsta = errno;
         cerror = strerror(errno);
         dLog(DLOG_MAJOR,"mq_init() : Error Emptying Message Queue for %s.",
                         ctaskname);
         dLog(DLOG_MAJOR,"          : Error Code %4d (%s)",errsta,cerror);
       }
       break;
     }
   }
 }


 return(m_id);

#if 0
 if ((m_id = msgget(msg_key, 0666 | IPC_CREAT)) == -1) 
		{
       	dLog(DLOG_MINOR"init(): Error Creating Message Queue for %s \n",taskname);
       	perror("Mssget:");
       	return(-1);
    	}

    return(m_id);
#endif

}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

int      gmqinfo_(int      * mid, int      * counts)
{
  int               status;
  int               index;
  struct msqid_ds   qid_buf;

  status = msgctl((*mid), IPC_STAT,&qid_buf);
  if(status == 0) {
    counts[0] = (int     ) qid_buf.msg_cbytes;
    counts[1] = (int     ) qid_buf.msg_qnum;
    counts[2] = (int     ) qid_buf.msg_qbytes;
    counts[3] = (int     ) qid_buf.msg_lspid;
    counts[4] = (int     ) qid_buf.msg_lrpid;
  }
  else {
    for(index=0;index<5;index++) {
      counts[index] = 0;
    }
  /*counts[0] = errno * (-1);*/
  }
  return(status);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

int      mq_dele_(int      * mid)
{
  int        mqid;
  int        status;
  int        ierror;
  char     * cerror;

  mqid   = (*mid);
  if(mqid == -1L) {
    mqid = get_msgid_();
  }

  if(mqid == 0L) {    /* If no queue establish do not process */
    status = 0L;
  }
  else {
    status = msgctl(mqid, IPC_RMID, (struct msqid_ds *) NULL);
    if(status < 0L) {
      ierror = errno;
      cerror = strerror(ierror);
      dLog(DLOG_MAJOR,"Error %d removing Message Queue %d (%s).",
              ierror,mqid,cerror);
      status = ierror;
    }
    else {
      status = 0L;
    }
  }
  return(status);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

int      getppid_(void)
{
  pid_t     gid;

  gid   =   getppid();
  
  return((int     ) gid);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

int      getpgid_(int      * f_pid)
{
  pid_t     pid;
  pid_t     gid;

  pid   =   (pid_t) (*f_pid);
  gid   =   getpgid(pid);
  
  return((int     ) gid);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/


int      setuid_(int      * f_pid)
{
  pid_t     pid;
  int       sta;

  pid   =   (pid_t) (*f_pid);
  sta   =   setuid(pid);
  if(sta < 0) {
    sta = errno;
  }
  
  return(sta);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

