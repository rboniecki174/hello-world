/* getmqbuf
 * This funtion returns the contents of the message queue
 * NOTE: assume outbuf is declared externally to be equal to the maximum number
 * of messages on a message queue, as configured for the system.
 */

// Revision   When       Who   Why
// tt60135    03/02/11   GJ    Replace obsolete realtime1.h with gb1_init.h

#include <stdio.h>
#include <sys/msg.h>

#include "private_data.h"
#include "gb1_init.h"
#include <dlogdefs.h>

//#define LOCDEBUG 1

void getmqbuf_( int *outbuf, int *maxbufsize, int *retsize )
{

#ifdef __linux__
  *retsize = 0;
  return;
#else
   size_t bufsize;
   struct msgsnap_head *buf;
   struct msgsnap_mhead *mhead;
   int mlen;
   int msqid = get_msgid_();
   int i;
   char * ptr;
   union _packet 
   {
            unsigned char  c1[4];  
            unsigned short i2[2];
            unsigned int   i4;
   } packet; 

#ifdef LOCDEBUG   
   dLog(DLOG_STDLOG,"Message Queue ID is %d",msqid);
#endif
   /* allocate a minimum-size buffer */
   buf = malloc(bufsize = sizeof(struct msgsnap_head));

   /* read all of the messages from the queue */
   for (;;) {
        if (msgsnap(msqid, buf, bufsize, 0) != 0) {
          perror("msgsnap");
          free(buf);
          return;
        }
#ifdef LOCDEBUG   
        if (buf->msgsnap_nmsg > 0) {
          dLog(DLOG_STDLOG,"head size =%d", buf->msgsnap_size);
          dLog(DLOG_STDLOG,"head num  =%d", buf->msgsnap_nmsg);
        }
#endif
        if (bufsize >= buf->msgsnap_size)  /* we got them all */
           break;
        /* we need a bigger buffer */
        buf = realloc(buf, bufsize = buf->msgsnap_size);
   }

   /* process each message in the queue (there may be none) */
   mhead = (struct msgsnap_mhead *)(buf + 1);  /* first message */
   for (i = 0; i < buf->msgsnap_nmsg; i++) {

     mlen = mhead->msgsnap_mlen;

     if (i >= *maxbufsize) {
       dLog(DLOG_MINOR,"%s: Warning, number of messages exceeds buffer size of %d",
         "getmqbuf",i);
       break;
     }

     ptr = (char *)(mhead+1);
     memcpy(&packet, ptr+4, 4);    /* this should be the packet */

     outbuf[i]=packet.i4; 

//   dLog(DLOG_MINOR,"BYTES: %03X %03X %03X %03X   ",
//       packet.c1[0], packet.c1[1], packet.c1[2], packet.c1[3]);
//   dLog(DLOG_MINOR,"SHORT: %04X %04X    ", packet.i2[0], packet.i2[1]);
//   dLog(DLOG_MINOR,"LONG: %08X\n", packet.i4);

//   dLog(DLOG_MINOR,"BYTES: %03d %03d %03d %03d   ",
//             packet.c1[0], packet.c1[1], packet.c1[2], packet.c1[3]);
//   dLog(DLOG_MINOR,"SHORT: %04d %04d    ", packet.i2[0], packet.i2[1]);
//   dLog(DLOG_MINOR,"LONG: %08d\n", packet.i4);
//            
#ifdef LOCDEBUG   
     dLog(DLOG_MINOR,"msg #%d, mhead=%d, mtype =%d, mlen=%d \n", 
             i, (int)mhead,  mhead->msgsnap_mtype, mlen);
#endif

/* advance to the next message header */
     mhead = (struct msgsnap_mhead *)
        ((char *)mhead + sizeof(struct msgsnap_mhead) +
           ((mlen + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1)));
   }
   free(buf);
   dLog(DLOG_STDLOG,"MQID %d size is %d", msqid, i); 
   *retsize = i-1; 
#endif    //else if not linux
}
