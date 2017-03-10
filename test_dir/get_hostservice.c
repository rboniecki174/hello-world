/********************************************************************************/
/*                                                                              */
/* get_hostservice.c   Get Host and Service Strings from a string     R00-01.00 */
/*                                                                              */
/* This file contains the routines used to extract the host name and service    */
/* name (port number etc.) from a string.  The expected format of the input     */
/* string is                                                                    */
/*                                                                              */
/*          host:service                                                        */
/*                                                                              */
/* The FORTRAN interface for this code is                                       */
/*                                                                              */
/*        CHARACTER*n  INPUT                                                    */
/*        CHARACTER*h  HOST                                                     */
/*        CHARACTER*s  SERVICE                                                  */
/*        INTEGER*4    STATUS                                                   */
/*                                                                              */
/*        CALL GET_HOSTPORT(INPUT,HOST,SERVICE,STATUS)                          */
/*                                                                              */
/* The "C" interface for this code is                                           */
/*                                                                              */
/*        unsigned char * input;                                                */
/*        unsigned char * host;                                                 */
/*        unsigned char * service;                                              */
/*        int             status;                                               */
/*                                                                              */
/*        status = get_hostport(input,host,service);                            */
/*                                                                              */
/* The status values returned by this routines are                              */
/*                                                                              */
/*     0  = Both an host and service (port) string have been extracted.         */
/*  -101  = A problem was found with the string.  More of generic error.        */
/*  -102  = A host name or IP address was not located.                          */
/*  -103  = The colon between the host and port strings is missing.             */
/*  -104  = The port or service string is missing.                              */
/*  -105  = One of the strings does not fit into the output field.              */
/*  -106  = The count string contains non-numeric characters                    */
/*                                                                              */
/*==============================================================================*/
/*                                                                              */
/* Programming Notes                                                            */
/*                                                                              */
/* The function g_host_port() does not null append or blank append the strings. */
/* This is to allow the function to be usable with both "C", FORTRAN or any     */
/* other potential language.  The callers of the "C" and FORTRAN interface      */
/* routines must initialize their work space as appropriate.                    */
/*                                                                              */
/* The "C" interface routine operates on the basis that the output strings host */
/* and service are at least 256 bytes in length.  This value is hardcoded.      */
/* The caller MUST preinitialize the buffers.                                   */
/*                                                                              */
/* The status values are 0 and -100 based to allow for callers of routines that */
/* use this code to be able to identify errors from the tcp connect routines.   */
/*                                                                              */
/* To build the test program use the following command line:                    */
/*                                                                              */
/*       cc -o get_hostservice -DMAIN -I ../include-gtn get_hostservice.c       */
/*                                                                              */
/*==============================================================================*/
/*                                                                              */
/* Revision History                                                             */
/*                                                                              */
/* Revision   When         Who       Why                                        */
/* =========  ===========  ========  ========================================== */
/* R00-00.00  24/Mar/1999  DRLJr     Routines created                           */
/* R00-01.00  16/Aug/2001  DRLJr     Added a connection count option and allowed*/
/*                                   for blanks around the strings.             */
/*                                                                              */
/********************************************************************************/

/*==============================================================================*/
/* Include Files                                                                */
/*==============================================================================*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*==============================================================================*/
/* Include Files                                                                */
/*==============================================================================*/

int      get_hostport(unsigned char * input,
                              unsigned char * host, 
                              unsigned char * port);

void get_hostport_(unsigned char * input,   unsigned char * host, 
                           unsigned char * port,    int           * status, 
                           int             length,  int             hlength,
                           int             slength);

int      get_hostport_count(unsigned char      * input,
                                    unsigned char      * host, 
                                    unsigned char      * port, 
                                    int                * count); 


/*==============================================================================*/
/* g_host_port()                                                                */
/*==============================================================================*/

#ifdef __cplusplus

extern "C" {

#endif

static int      g_host_port(unsigned char     * input,   /* Input string            */
                            unsigned char     * host,    /* Host name/IP Address    */
                            unsigned char     * service, /* Service Name/Port Numb. */
                            int               * count,   /* Number of connections   */
                            int                 length,  /* Length of input         */
                            int                 hlength, /* Length of host field    */
                            int                 slength);/* Length of service field */
#ifdef __cplusplus

};

#endif

/*==============================================================================*/
/* g_host_port()                                                                */
/*==============================================================================*/

static int      g_host_port(unsigned char     * input,   /* Input string            */
                            unsigned char     * host,    /* Host name/IP Address    */
                            unsigned char     * service, /* Service Name/Port Numb. */
                            int               * count,   /* Number of connections   */
                            int                 length,  /* Length of input         */
                            int                 hlength, /* Length of host field    */
                            int                 slength) /* Length of service field */
{
  int                    status;
  int                    offset;
  int                    wcount;
  int                    movlen;
  int                    strt_host,   stop_host,   leng_host;
  int                    strt_service,stop_service,leng_service;
  int                    strt_count,  stop_count,  leng_count,   clength;
  unsigned char          count_str[32];
  unsigned char        * count_stop;

  /*----------------------------------------------------------------------------*/
  /* Processing                                                                 */
  /*----------------------------------------------------------------------------*/

  if(input == (unsigned char *) NULL) {
    status = -101L;
    goto exit_here;
  }
  status         =  0L;
  strt_host      = -1L;
  stop_host      = -1L;
  strt_service   = -1L;
  stop_service   = -1L;
  strt_count     = -1L;
  stop_count     = -1L;
  clength        = sizeof(count_str) - 1L;

  /*----------------------------------------------------------------------------*/
  /* Locate the start of the host name/IP address                               */
  /*----------------------------------------------------------------------------*/

  for(offset=0L;;offset++) {
    if(offset >= length) {
      status = -101L;
      goto exit_here;
    }
    if(input[offset]==':') {
      status = -102L;
      goto exit_here;
    }
    if((input[offset]!=' ') && (input[offset]!='\t')) {
      strt_host   = offset;
      break;
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the end of the host name/IP address                                 */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      status = -101L;
      goto exit_here;
    }
    if(input[offset]==':') {
      if(stop_host < 0L) {
        stop_host = offset - 1L;
      }
      offset++;
      break;
    }
    if((input[offset]==' ') || (input[offset]=='\t')) {
      if(stop_host < 0L) {
        stop_host = offset - 1L;
      }
    /*break;*/
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the start of the service name/port number                           */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      status = -103L;
      goto exit_here;
    }
#if 0
    if(input[offset]==':') {
      offset++;
      break;
    }
#endif
    if((input[offset]!=' ') && (input[offset]!='\t')) {
    /*status = -103L;*/
      break;
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the start of the service name/port number                           */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      status = -104L;
      goto exit_here;
    }
    if((input[offset]!=' ') && (input[offset]!='\t')) {
      strt_service = offset;
      break;
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the end of the host name/IP address                                 */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      stop_service = offset - 1L;
      goto mv_data;
      break;
    }
    if(input[offset]==':') {
      if(stop_service < 0L) {
        stop_service = offset - 1L;
      }
      offset++;
      break;
    }
    if((input[offset]==' ') || (input[offset]=='\t')) {
      if(stop_service < 0L) {
        stop_service = offset - 1L;
      }
    /*break;*/
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the start of the number of connections                              */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      goto mv_data;
    }
    if((input[offset]!=' ') && (input[offset]!='\t')) {
      strt_count = offset;
      break;
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Locate the end of the number of connections                                */
  /*----------------------------------------------------------------------------*/

  for(;;offset++) {
    if(offset >= length) {
      stop_count   = offset - 1L;
      goto mv_data;
      break;
    }
    if(input[offset]==':') {
      if(stop_count   < 0L) {
        stop_count   = offset - 1L;
      }
      offset++;
      break;
    }
    if((input[offset]==' ') || (input[offset]=='\t')) {
      if(stop_count   < 0L) {
        stop_count   = offset - 1L;
      }
    /*break;*/
    }
  }

  /*----------------------------------------------------------------------------*/
  /* Move the strings to the output strings                                     */
  /*----------------------------------------------------------------------------*/

  mv_data :

  leng_host    =  stop_host     - strt_host     + 1L;
  leng_service =  stop_service  - strt_service  + 1L;
  leng_count   =  stop_count    - strt_count    + 1L;

  if(leng_host <= hlength) {
    if(leng_host <= 0L) {
      movlen =   -1L;
      status = -105L;
    }
    else {
      movlen = leng_host;
    }
  }
  else {
    movlen =   -1L;
    status = -105L;
  }
  if(host != (unsigned char *) NULL) {
    if(movlen > 0L) {
      memcpy(host,&(input[strt_host]),movlen);
    }
  }

  if(leng_service <= slength) {
    if(leng_service <= 0L) {
      movlen =   -1L;
      status = -105L;
    }
    else {
      movlen = leng_service;
    }
  }
  else {
    movlen = slength;
    status = -105L;
  }
  if(service != (unsigned char *) NULL) {
    if(movlen > 0L) {
      memcpy(service,&(input[strt_service]),movlen);
    }
  }

  if(strt_count >= 0L) {
    if(leng_count   <= clength) {
      movlen = leng_count;
    }
    else {
      movlen = clength;
      status = -105L;
    }
    memset(count_str,0x00,sizeof(count_str));
    memcpy(count_str,&(input[strt_count]),movlen);
    wcount = strtol((const char *)count_str,(char **)&count_stop,10L);
    if((*count_stop) != '\0') {
      status = -106L;
    }
    if(wcount <= 0L) {
      wcount = 1L;
    }
  }
  else {
    wcount = 1L;
  }
  if(count != (int             *) NULL) {
    (*count) = wcount;
  }

  /*----------------------------------------------------------------------------*/
  /* Return to caller with the appropriate status                               */
  /*----------------------------------------------------------------------------*/

  exit_here :

#if 0
#ifdef MAIN

  fprintf(stderr,"Host %2d - %2d, Port %2d - %2d, Count %2d - %2d, Status %2d, ",
          strt_host,stop_host,strt_service,stop_service,strt_count,stop_count,status);
  fprintf(stderr,"Input <%s>, ",input);
  fprintf(stderr,"Host <%s>, Service <%s>, Count <%5d>",host,service,(*count));
  fprintf(stderr,"\n");

#endif
#endif

  return(status);
}


/*==============================================================================*/
/* get_hostport() - "C" Interface                                               */
/*==============================================================================*/

int      get_hostport(unsigned char * input, unsigned char * host, 
                                                     unsigned char * port)
{
  int               status;                 /* Status to return to caller       */
  int               length;                 /* Length of the input string       */
  int               hlength;                /* Maximum length of host string    */
  int               slength;                /* Maximum length of service string */
  int               count;                  /* The number of connections        */

  if(input != (unsigned char *) NULL) {
    length          = strlen((char *)input);
    hlength         = 256L;
    slength         = 256L;
    status          = g_host_port(input,host,port,&count,length,hlength,slength);
  }
  else {
    status = -101L;
  }
  return(status);
}

/*==============================================================================*/
/* get_hostport_count() - "C" Interface                                         */
/*==============================================================================*/

int      get_hostport_count(unsigned char      * input,
                                    unsigned char      * host,
                                    unsigned char      * port,
                                    int                * count)
{
  int               status;                 /* Status to return to caller       */
  int               length;                 /* Length of the input string       */
  int               hlength;                /* Maximum length of host string    */
  int               slength;                /* Maximum length of service string */

  if(input != (unsigned char *) NULL) {
    length          = strlen((char *)input);
    hlength         = 256L;
    slength         = 256L;
    status          = g_host_port(input,host,port,count,length,hlength,slength);
  }
  else {
    status = -101L;
  }
  return(status);
}

/*==============================================================================*/
/* get_hostport() - "FORTRAN" Interface                                         */
/*==============================================================================*/

void get_hostport_(unsigned char * input,   unsigned char * host,
                           unsigned char * port,    int           * status,
                           int             length,  int            hlength,
                           int             slength)
{
  int               count;                  /* The number of connections        */

  (*status)      = g_host_port(input,host,port,&count,length,hlength,slength);
  return;
}


/*==============================================================================*/
/* Test program                                                                 */
/*==============================================================================*/

#ifdef MAIN

int      main(int      argc, unsigned char ** argv)
{
  int         index;
  int         status;
  int         length;
  int         count;
  unsigned char   host[256];
  unsigned char   port[256];

  for(index=1;index<argc;index++) {

    fprintf(stderr,"\n");
    fprintf(stderr,"\n");

    length = strlen(argv[index]);
    memset(host,0x00,256);
    memset(port,0x00,256);
    count  = 0L;
    status = g_host_port(argv[index],host,port,&count,length, 32, 32);
    fprintf(stderr,"g_host_port        <%s>, ",argv[index]);
    fprintf(stderr,"Host <%s>, port <%s>, Count <%5d>, Status <%5d>",host,port,count,status);
    fprintf(stderr,"\n");

    length = strlen(argv[index]);
    memset(host,0x00,256);
    memset(port,0x00,256);
    count  = 0L;
    status = get_hostport(argv[index],host,port);
    fprintf(stderr,"get_hostport       <%s>, ",argv[index]);
    fprintf(stderr,"Host <%s>, port <%s>, Count <%5d>, Status <%5d>",host,port,count,status);
    fprintf(stderr,"\n");

    length = strlen(argv[index]);
    memset(host,0x00,256);
    memset(port,0x00,256);
    count  = 0L;
    status = get_hostport_count(argv[index],host,port,&count);
    fprintf(stderr,"get_hostport_count <%s>, ",argv[index]);
    fprintf(stderr,"Host <%s>, port <%s>, Count <%5d>, Status <%5d>",host,port,count,status);
    fprintf(stderr,"\n");

    count  = 0L;
    (void)   get_hostport_(argv[index],host,port,&status,length,32,32);
    fprintf(stderr,"get_hostport_      <%s>, ",argv[index]);
    fprintf(stderr,"Host <%s>, port <%s>, Count <%5d>, Status <%5d>",host,port,count,status);
    fprintf(stderr,"\n");

  }
  fprintf(stderr,"\n");
  fprintf(stderr,"\n");
  exit(0L);
}

#endif

/********************************************************************************/
/*                                                                              */
/********************************************************************************/
