/**********************************************************************/
/* set_socket_options.c                                               */
/*                                                                    */
/* 29/Jul/2004 jeffs Move set_socket_options to its own file from     */
/*                   tcp_connect.o to avoid multi defines with gtnsdk */
/*                                                                    */
/**********************************************************************/
/*                                                                    */
/**********************************************************************/
/* Include Files and #defines                                         */
/**********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/time.h>
#include <ulimit.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dlogdefs.h>

#define SOCKET_BUFR_SIZE       1024
#define SOCKET_BUFR_SIZE_MIN    512

#define FDNULL   ((fd_set *) NULL)
#define TOUTNULL ((struct timeval *) NULL)
/**********************************************************************/
/* set_socket_options                                                 */
/*                                                                    */
/* sockfd      : This is the Unix file descriptor of the socket       */
/*             : connection that is to have its buffer size changed.  */
/* bufr_size   : This is the desired size of the socket buffers.      */
/*                                                                    */
/**********************************************************************/

static char   sock_err_msg[] = {
 "set_socket_options : Unable to set %s buffer size, Error %d (%s)."
};

int      set_socket_options(int      sockfd, int      bufr_size)
{
  int        * adr_bufrsize;
  int          bufrsize;
  int          sizelint;          /* Size of int        */
  int          status_snd;
  int          status_rcv;
  char       * errstring;

  bufrsize     = bufrsize;
  adr_bufrsize = &(bufrsize);
  sizelint     = sizeof(int     );

  if(bufrsize <= 0) {
    bufrsize = SOCKET_BUFR_SIZE;
  }
  else if(bufrsize < SOCKET_BUFR_SIZE_MIN) {
    bufrsize = SOCKET_BUFR_SIZE_MIN;
  }

  status_snd   = setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,adr_bufrsize,sizelint);
  if(status_snd != 0) {
    status_snd = errno;
    errstring  = strerror(status_snd);
    dLog(DLOG_MAJOR,sock_err_msg,"SO_SNDBUF",errno,errstring);
  }
  status_rcv   = setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,adr_bufrsize,sizelint);
  if(status_rcv != 0) {
    status_rcv = errno;
    errstring  = strerror(status_rcv);
    dLog(DLOG_MAJOR,sock_err_msg,"SO_RCVBUF",errno,errstring);
  }

  return(0L);
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

