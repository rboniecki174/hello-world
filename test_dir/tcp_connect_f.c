/**********************************************************************/
/* tcp_connect_f.c                                         R00-00.00  */
/*                                                                    */
/* 16/Apr/1997 DRLJr Changed open_tcp_as_server_ to return a -1 in    */
/*                   UNIX FD argument instead of exiting with a zero  */
/*                   exit status via exit(0).  Changed printf to      */
/*                   dLog(DLOG_MINOR ...). Also changed                */
/*                   open_tcp_as_client                               */
/*                                                                    */
/* 27/Mar/1998 DRLJr Moved FORTRAN Interface rooutines to a file by   */
/*                   themself to allow "C" program to not be forced   */
/*                   to included unneeded code.                       */
/*                                                                    */
// 22/May/2014 gj    Clean up prototypes
//
/*====================================================================*/
/*                                                                    */
/* Status values for server_only_connection and client_open_connection*/
/* returned to the caller.                                            */
/*                                                                    */
/*  > 0 = The fd that is used to connect the socket.                  */
/*   -1 = socket() call failed                                        */
/*   -2 = setsockopt() call failed                                    */
/*   -3 = bind() call failed                                          */
/*   -4 = listen() call failed                                        */
/*   -5 = accept() call failed                                        */
/*   -6 = gethostbyname() call failed                                 */
/*   -7 = connect() call failed                                       */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "dlogdefs.h"
//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

static char   taskname[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',0,0,0,0 };

#define TASKNAME() get_tname_(taskname,8L)
#define MAXSTRING  256

#define SOCKET_BUFR_SIZE       1024
#define SOCKET_BUFR_SIZE_MIN    512

static char sockerr[]
 = { "%8.8s : Unable to make %6.6s connection - subroutine %s failed - errno=%5d"};
static char hostserv[]
 = { "%8.8s : Making TCP Connection - HOST: %s, SERVICE: %s" };
static char conn_ok[] = {"%8.8s : %6.6s Connection Established"};

static char  * sub_name[] = { "", "socket()", "setsockopt()",
                                  "bind()",   "listen()",
                                  "accept()", "getbyhostname()",
                                  "connect",
                              0x00 };

int      brkenv_server_open_connection(char *host, char *service);
int      brkenv_client_open_connection(char *host, char *service);
int      set_socket_options(int      sockfd, int      bufr_size);


/*####################################################################*/
/* open_tcp_as_server_                                                */
/*####################################################################*/

void open_tcp_as_server_(int      * tcpfd,
                         char     * inhost, 
                         char     * inservice,
                         int        len_inhost,
                         int        len_inservice )
                               
{
 char        hostname[MAXSTRING];
 char        servicename[MAXSTRING];
 int         cnt;
 int         status;
 int         maxlen;

 TASKNAME();

 maxlen = ((len_inhost > MAXSTRING) ? MAXSTRING - 1 : len_inhost);
 for(cnt=0;cnt<maxlen;cnt++) {
   if(inhost[cnt] == ' ') {
     break;
   }
   hostname[cnt] = inhost[cnt];
 }
 hostname[cnt]   = '\0';

 maxlen = ((len_inservice > MAXSTRING) ? MAXSTRING - 1 : len_inservice);
 for(cnt=0;cnt<maxlen;cnt++) {
   if(inservice[cnt] == ' ') {
     break;
   }
   servicename[cnt] = inservice[cnt];
 }
 servicename[cnt] = '\0';

 dLog(DLOG_STDLOG,hostserv,taskname,hostname,servicename);
 dLog(DLOG_STDOUT,hostserv,taskname,hostname,servicename);

 status = brkenv_server_open_connection(hostname,servicename);
 if(status < 0) {
   status *= (-1);
   dLog(DLOG_MAJOR,sockerr,taskname,"Server",sub_name[status],errno);
   dLog(DLOG_STDERR,sockerr,taskname,"Server",sub_name[status],errno);
   (*tcpfd)   = -1L;
 }
 else {
   (*tcpfd)   = status;
   status     = set_socket_options((*tcpfd),0L);
   dLog(DLOG_STDLOG,conn_ok,taskname,"Server");
   dLog(DLOG_STDOUT,conn_ok,taskname,"Server");
 }

 return;

}

/*####################################################################*/
/* open_tcp_as_client_                                                */
/*####################################################################*/

void open_tcp_as_client_(int      * tcpfd,
                         char     * inhost, 
                         char     * inservice,
                         int        len_inhost,
                         int        len_inservice )
                               
{
 char        hostname[MAXSTRING];
 char        servicename[MAXSTRING];
 int         cnt;
 int         status;
 int         maxlen;

 TASKNAME();

 maxlen = ((len_inhost > MAXSTRING) ? MAXSTRING - 1 : len_inhost);
 for(cnt=0;cnt<maxlen;cnt++) {
   if(inhost[cnt] == ' ') {
     break;
   }
   hostname[cnt] = inhost[cnt];
 }
 hostname[cnt]   = '\0';

 maxlen = ((len_inservice > MAXSTRING) ? MAXSTRING - 1 : len_inservice);
 for(cnt=0;cnt<maxlen;cnt++) {
   if(inservice[cnt] == ' ') {
     break;
   }
   servicename[cnt] = inservice[cnt];
 }
 servicename[cnt] = '\0';

 dLog(DLOG_STDLOG,hostserv,taskname,hostname,servicename);
 dLog(DLOG_STDOUT,hostserv,taskname,hostname,servicename);

 status = brkenv_client_open_connection(hostname,servicename);
 if(status < 0) {
   status *= (-1);
   dLog(DLOG_MAJOR,sockerr,taskname,"Client",sub_name[status],errno);
   dLog(DLOG_STDERR,sockerr,taskname,"Client",sub_name[status],errno);
   (*tcpfd)   = -1L;
 }
 else {
   (*tcpfd)   = status;
   status     = set_socket_options((*tcpfd),0L);
   dLog(DLOG_STDLOG,conn_ok,taskname,"Client");
   dLog(DLOG_STDOUT,conn_ok,taskname,"Client");
 }

 return;

}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/

