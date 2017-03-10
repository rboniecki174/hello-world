/**********************************************************************/
/* tcp_connect.c                                           R02-03.03  */
/*                                                                    */
/* 16/Apr/1997 DRLJr Changed open_tcp_as_server_ to return a -1 in    */
/*                   UNIX FD argument instead of exiting with a zero  */
/*                   exit status via exit(0).  Changed printf to      */
/*                   dLog(DLOG_MINOR ...). Also changed                */
/*                   open_tcp_as_client                               */
/*                                                                    */
/* 19/Jun/1997 DRLJr Recoded various subroutines.                     */
/* 24/Jun/1997 DRLJr Added Routine server_open_multiconnections       */
/* 22/Jan/1998 DRLJr Added Routine to set socket options. (R01-01.00) */
/* 27/Mar/1998 DRLJr Moved FORTRAN interface routines to a new file   */
/*                   so that "C" programs do not include unneeded     */
/*                   code. (R02-00.00)                                */
/* 02/Mar/1999 DRLJr If the socket was connected but an error status  */
/*                   is being recorded then insure the socket is      */
/*                   first closed.  (R02-01.00)                       */
/* 15/Mar/1999 DRLJr Close the socket fd after the connection has     */
/*                   been made in server_open_connection().(R02-02.00)*/
/* 17/Mar/1999 DRLJr Added routines server_open_peerconnection(),     */
/*                   server_wait_for_peerconnection() and the routine */
/*                   server_accept_peerconnection() as well as other  */
/*                   routines.                         (R02-03.00)    */
/* 24/Mar/1999 DRLJr Added documentation. (R02.03.01)                 */
/* 20/Apr/1999 DRLJr Added documentation. Also added a close if()     */
/*                   block for peer-to-peer open failures. (R02-03.02)*/
/* 13/Jan/2000 DRLJr Added ability of client to use ip addresses as   */
/*                   well as hostnames.                               */
/* 13/Feb/2001 jeffsun Port to Sun. Remove unsigned from cast in      */
/*                   gethostbyaddr R03.00.00                          */
/* 21/Jul/2004 jeffs Move set_socket_options to its own file          */
// 22/May/2014 gj    tt80093 - Clean up prototypes
/*                                                                    */
/*====================================================================*/
/*                                                                    */
/* Status values for server_only_connection and client_open_connection*/
/* returned to the caller.   This codes are also used by some of the  */
/* other routines.                                                    */
/*                                                                    */
/*  > 0 = The fd that is used to connect the socket.                  */
/*   -1 = socket() call failed                                        */
/*   -2 = setsockopt() call failed                                    */
/*   -3 = bind() call failed                                          */
/*   -4 = listen() call failed                                        */
/*   -5 = accept() call failed                                        */
/*   -6 = gethostbyname() or gethostbyaddr() call failed              */
/*   -7 = connect() call failed                                       */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/
/* Include Files and #defines                                         */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <math.h>

#include <sys/types.h>
#include <sys/time.h>
#include <ulimit.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

#define SOCKET_BUFR_SIZE       1024
#define SOCKET_BUFR_SIZE_MIN    512

#define FDNULL   ((fd_set *) NULL)
#define TOUTNULL ((struct timeval *) NULL)
#if 1

/**********************************************************************/
/* server_open_connection                                             */
/*                                                                    */
/* host      : This is a character string containing the host name    */
/*           : or IP address of the host system.                      */
/* service   : This is a character string containing the port number  */
/*           : or service name.                                       */
/*                                                                    */
/* The function returns either one of the error statuses listed above */
/* or the unix file descriptor of the open connection.  The original  */
/* socket file descriptor remains open.                               */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_open_connection(char *host, char *service)
{
 struct sockaddr_in    serv_addr;
 struct sockaddr_in    client;
 int                   addr_len;
 int                   sockfd;
 int                   confd;
 int                   sopnum;
 int                   status;
 int  val5000=1024;

 confd      = -1L;
 sockfd     = -1L;
 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 status               = socket(AF_INET,SOCK_STREAM,0);
 if(status >= 0) {
   sockfd   = status;
   sopnum   = atoi(service);        
   status   = setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR,
                         (int      *) &sopnum, sizeof(int     ));
   if(status >= 0) {
     bzero((char *)&serv_addr, sizeof(serv_addr));
     serv_addr.sin_family      = AF_INET;
     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     serv_addr.sin_port        = htons((unsigned short) atoi(service));
     status = bind(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
     if(status >= 0) {
       status = listen(sockfd, 1L);
       if(status >= 0) {
         addr_len  = sizeof(client);
         status    = accept(sockfd, (struct sockaddr *) &client, 
                                    (unsigned int    *) &addr_len);
         if(status > 0) {
           confd   = status;
           status   = setsockopt(sockfd,SOL_SOCKET, SO_SNDBUF,   
                        &val5000, sizeof(int));
         /*dLog(DLOG_MINOR,"status on SO_SNDBUF:=%d",status);*/
           status   = setsockopt(sockfd,SOL_SOCKET, SO_RCVBUF,   
                        &val5000, sizeof(int));
         /*dLog(DLOG_MINOR,"status on SO_RCVBUF:=%d",status);*/
           /*---------------------------------------------------*/
           /* Close the file descriptor used by the socket call */
           /* to establish the host/port combination.  Since    */
           /* the caller does not have access to the file       */
           /* descriptor and this is a peer-to-peer type        */
           /* connection we do not need it left open.  Also if  */
           /* the caller closed the data file descriptor (confd)*/
           /* sockfd needs to be closed if the program is to be */
           /* able to reopen the host/port combination without  */
           /* having to terminate and restart.                  */
           /*---------------------------------------------------*/
           status  = close(sockfd);    /* Close the socket fd   */
           sockfd  = -1L;              /* Indicate it is closed */
           status  = 0;
         }
         else {
           status = -5L;
         }
       }
       else {
         status = -4L;
       }
     }
     else {
       status = -3L;
     }
   }
   else {
     status = -2L;
   }
 }
 else {
   status   = -1L;
 }
#if 0
 dLog(DLOG_STDLOG,"Socket Connection (SOC) : status=%4d, confd=%4d.",status,confd);
#endif
 if(status == 0) {
   return(confd);
 }

 /*-------------------------------------------------------------------*/
 /* At this point we are returning an error status.  We will close    */
 /* file descriptor that the socket() call used to bind the host and  */
 /* port (service).  This will allow for the calling program to make  */
 /* a new attempt at opening the same host/port combination.  If this */
 /* is not done then the program must first terminate and restart if  */
 /* the particular host/port combination is to be re-used.  This code */
 /* is intended to be used for peer-to-peer type connections.         */
 /*-------------------------------------------------------------------*/

 if(confd >= 0) {            /* Connection fd is still open           */
   close(confd);             /* Insure the socket is closed           */
 }                           /*                                       */
 if(sockfd >= 0) {           /* Original socket is still open         */
   close(sockfd);            /* Insure the socket is closed           */
 }                           /*                                       */
 return(status);             /* Return an error status                */
}                            /*                                       */


/**********************************************************************/
/* server_open_multiconnections                                       */
/*                                                                    */
/* host        : This is a character string containing the host       */
/*             : name or IP address of the host system.               */
/* service     : This is a character string containing the port       */
/*             : number or service name.                              */
/* connections : This is the number of connections that may be        */
/*             : back logged before an error is reported to the other */
/*             : end of the socket connection.                        */
/*                                                                    */
/* The function returns either one of the error statuses listed above */
/* or the unix file descriptor of the open socket.                    */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_open_multiconnections(char *host, char *service,
                                      int      connections)
{
 struct sockaddr_in    serv_addr;
 int                   sockfd;
 int                   confd;
 int                   sopnum;
 int                   status;

 confd      = -1L;
 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 status               = socket(AF_INET,SOCK_STREAM,0);
 if(status >= 0) {
   sockfd   = status;
   sopnum   = atoi(service);        
   status   = setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR,
                         (int      *) &sopnum, sizeof(int     ));
   if(status >= 0) {
     bzero((char *)&serv_addr, sizeof(serv_addr));
     serv_addr.sin_family      = AF_INET;
     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     serv_addr.sin_port        = htons((unsigned short)atoi(service));
     status = bind(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
     if(status >= 0) {
       status = listen(sockfd, connections);
       if(status >= 0) {
       }
       else {
         status = -4L;
       }
     }
     else {
       status = -3L;
     }
   }
   else {
     status = -2L;
   }
 }
 else {
   status   = -1L;
 }
#if 0
 dLog(DLOG_STDLOG,"Socket Connection (SOM) : status=%4d, confd=%4d.",status,confd);
#endif
 if(status == 0) {
   return(sockfd);
 }
 return(status);
}


/**********************************************************************/
/* server_wait_for_connection                                         */
/*                                                                    */
/* The tmout parameter is used in the following manner:               */
/*                                                                    */
/*   tmout   <  0 : Have the select wait forever or as int  as needed */
/*                : to get a connection.                              */
/*   tmout   == 0 : Return immediately if no connection is available. */
/*   tmout   >  0 : Wait for the specified number of milliseconds or  */
/*                : for a connection to occur whichever occurs first. */
/*                                                                    */
/*                                                                    */
/* The returned statuses for this routine are:                        */
/*                                                                    */
/*   status  <  0 : An error occured in the call.  The returned value */
/*                : is the negative value of error (errno * -1).      */
/*   status  == 0 : There is an except on the socket connection       */
/*                : operation.                                        */
/*   status  == 1 : There is a connection request pending from the    */
/*                : other end of the socket.                          */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_wait_for_connection(int      sockfd,int      tmout)
{
  int              status;          /* Status to return to caller     */
  int              maxfd;           /* Maximum fd in the set          */
  int              secs;            /* Work variable for seconds      */
  int              msecs;           /* Work variable for millseconds  */
  fd_set           rset;            /* Accept file descriptor set     */
  fd_set           eset;            /* Exception file descriptor set  */
  fd_set         * arset;           /* Address of rset                */
  fd_set         * aeset;           /* Address of eset                */
  struct timeval   tout;            /* Select Timeout structure       */
  struct timeval * a_tout;          /* Address of tout                */

  if(tmout <  0L) {
    a_tout = TOUTNULL;              /* Wait forever                   */
  }                                 /*                                */
  else if(tmout == 0L) {            /* Check if data is present       */
    a_tout       = &tout;           /* Set the address                */
    tout.tv_sec  = 0L;              /* Set the timeout                */
    tout.tv_usec = 0L;              /* Set the timeout                */
  }                                 /*                                */
  else {                            /* Wait for some time             */
    a_tout       = &tout;           /* Set the address                */
    secs         = tmout / 1000;    /* Number of seconds              */
    msecs        = tmout % 1000;    /* Number of milliseconds         */
    tout.tv_sec  = secs;            /* Set the timeout                */
    tout.tv_usec = msecs * 1000;    /* Set the timeout                */
  }                                 /*                                */

  arset          = &rset;           /* Address of rset                */
  aeset          = &eset;           /* Address of eset                */
  maxfd          = sockfd + 1;      /* Set the maximum fd set         */

  do_select :

  FD_ZERO(arset);                   /* Zero the file descriptor set   */
  FD_ZERO(aeset);                   /* Zero the file descriptor set   */

  FD_SET(sockfd,arset);             /* Set the socket fd in place     */
  FD_SET(sockfd,aeset);             /* Set the socket fd in place     */

  status = select(maxfd,arset, FDNULL,aeset, a_tout);

  if(status < 0L) {                 /* Error detected                 */

    if(errno == EAGAIN) {           /* If try again make a timeout    */
      errno  =  ETIME;              /* Fixup errno                    */
      status =  ETIME * (-1L);      /* Select a timeout               */
    }                               /*                                */
    else if(errno == EINTR) {       /* Go back and check again        */
      errno  =  0L;                 /* Clear it to zero               */
      goto do_select;               /*                                */
    }                               /*                                */
    else {                          /*                                */
      status = errno * (-1L);       /* Return a negative errno value  */
    }                               /*                                */
  }                                 /*                                */

  else if(status > 0L) {            /* Is ready or has exception      */

    status = FD_ISSET(sockfd,aeset);  /* Exception detected ?         */

    if(status == 0L) {              /* No exception check read        */
      status = FD_ISSET(sockfd,arset);  /* Accept ready ?             */
      if(status != 0L) {            /* It is ready                    */
        status = +1L;               /* Accept is ready                */
      }                             /*                                */
      else {                        /* Make into a timeout            */
        errno  =  ETIME;            /* Fixup errno                    */
        status =  ETIME * (-1L);    /* Set to timeout                 */
      }                             /*                                */
    }                               /*                                */

    else {                          /* Exception detected             */

      errno  =  EIO;                /* Flag as an I/O error/exception */
      status =  0L;                 /* Return exception status        */

    }                               /*                                */
  }                                 /*                                */

  else {                            /* Time out - status is 0         */
    errno    =  ETIME;              /* Fixup errno                    */
    status   =  ETIME * (-1L);      /* Set to TIMEOUT                 */
  }                                 /*                                */

  return(status);                   /* Return the status              */ 
}                                   /*                                */


/**********************************************************************/
/* server_accept_connection                                           */
/*                                                                    */
/* sockfd     : This is the UNIX file descriptor of the socket        */
/*            : that is used to accept a remote connection.           */
/*                                                                    */
/* This routine will return a unix file descriptor which is used with */
/* open connection or one of the status codes listed above.           */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_accept_connection(int      sockfd)
{
 struct sockaddr_in    client;
 int                   addr_len;
 int                   status;
 int                   confd;

 confd      = -1L;
 addr_len =  sizeof(client);
 status   =  accept(sockfd, (struct sockaddr *) &client,
                            (unsigned int    *) &addr_len);
 if(status >= 0) {
   confd  =  status;
   status =  0;
 }
 else {
   status =  -5L;
 }
#if 0
 dLog(DLOG_STDLOG,"Socket Connection (SAC) : status=%4d, confd=%4d.",status,confd);
#endif
 if(status == 0) {
   return(confd);
 }
 if(confd >= 0) { 
   close(confd);             /* Insure the socket is closed           */
 }
 return(status);
}


/**********************************************************************/
/* server_open_peerconnection                                         */
/*                                                                    */
/* host      : This is a character string containing the host name    */
/*           : or IP address of the host system.                      */
/* service   : This is a character string containing the port number  */
/*           : or service name.                                       */
/*                                                                    */
/* This routine returns the Unix file descriptor of the socket that   */
/* is used to start the process to make the socket connection.        */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_open_peerconnection(char *host, char *service)
{
  int         status;

  status = brkenv_server_open_multiconnections(host,service,1L);
  return(status);
}

/**********************************************************************/
/* server_wait_for_peerconnection                                     */
/*                                                                    */
/* sockfd   : This is the Unix file descriptor returned by the        */
/*          : routine server_open_peerconnection() used for the       */
/*          : connection.                                             */
/* tmout    : This is the timeout parameter to be used to wait for    */
/*          : connection.  Refer to server_wait_for_connection() for  */
/*          : details.                                                */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_wait_for_peerconnection(int      sockfd,int      tmout)
{
  int         status;

  status    = brkenv_server_wait_for_connection(sockfd,tmout);
  return(status);
}

/**********************************************************************/
/* server_accept_peerconnection                                       */
/*                                                                    */
/* sockfd   : This is the Unix file descriptor returned by the        */
/*          : routine server_open_peerconnection() used for the       */
/*          : connection.                                             */
/*                                                                    */
/* The routine returns the Unix File descriptor of the connection or  */
/* one of the status codes listed above.                              */
/*                                                                    */
/* This routine CLOSES the socket that is attached to the variable    */
/* sockfd since this routine is operating as a peer-to-peer           */
/* connection environment on a successful accept call.                */
/*                                                                    */
/**********************************************************************/

int      brkenv_server_accept_peerconnection(int      sockfd)
{
  int         status;
  int         clstat;

  status = brkenv_server_accept_connection(sockfd); /* Accept the connection */
  if(status >= 0L) {                         /* If successful         */
    clstat = close(sockfd);                  /* Release the socket    */
    if(clstat < 0L) {                        /* Close error?          */
    }                                        /*                       */
  }                                          /*                       */

  return(status);                            /* Return to the caller  */
}

#endif


/**********************************************************************/
/* client_open_connection                                             */
/*                                                                    */
/* host      : This is a character string containing the host name    */
/*           : or IP address of the host system.                      */
/* service   : This is a character string containing the port number  */
/*           : or service name.                                       */
/*                                                                    */
/* The function returns either one of the error statuses listed above */
/* or the unix file descriptor of the open connection.                */
/*                                                                    */
/**********************************************************************/
#if 1
int      brkenv_client_open_connection(char *host, char *service)
{
 struct hostent      * hp;
 struct sockaddr_in    serv_addr;
 unsigned int          ipaddr;
 int                   confd;
 int                   status;
 int optval = 1;

 confd      = -1L;
 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;

 ipaddr = inet_addr(host);
 if((int            ) ipaddr != -1L) {
    hp = gethostbyaddr((char *) &ipaddr,sizeof(unsigned int     ),AF_INET);
 }
 else {
    hp = gethostbyname(host);
 }
 if(hp != (struct hostent *) NULL) {
   bcopy(hp->h_addr, &serv_addr.sin_addr.s_addr, hp->h_length);
   serv_addr.sin_port = htons((unsigned short)atoi(service));
   confd              = socket(AF_INET, SOCK_STREAM,0);
   if(confd >= 0) {
     setsockopt(confd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
//   dLog(DLOG_STDLOG,"brkenv_client_open_connection: setting TCP_NODELAY");
     status = connect(confd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
     if(status >= 0) {
       status = 0L;
     }
     else {
       status = -7L;
     }
   }
   else {
     status = -1L;
   }
 }
 else {
   status  = -6L;
 }
#if 0
 dLog(DLOG_STDLOG,"Socket Connection (COC) : status=%4d, confd=%4d.",status,confd);
#endif
 if(status == 0L) {
   return(confd);
 }
 if(confd >= 0) { 
   close(confd);             /* Insure the socket is closed           */
 }
 return(status);
}
#endif

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

