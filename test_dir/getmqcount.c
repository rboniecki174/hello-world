/* getmqcount
 *  60634 This function returns a count of the number of messages on my msg queue. 
 *  Argument is the mqid passed in.   If mqid is zero then use own mqid.
 */ 

#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include "dlogdefs.h"
int      get_msgid_(void);

int getmqcount(int mqid)
{
      int stat;
      struct msqid_ds buf;

      if (mqid == 0 ) {
         mqid = get_msgid_();
         if (mqid == 0) {
            dLog(DLOG_WARNING,"Notice:Count of mq items could not be made since the mq is not in RT table for my process.");
            return (-1); 
         }
      }
      stat =  msgctl( mqid, IPC_STAT, &buf );
      if (stat == -1) {
         dLog(DLOG_MAJOR,"msgctl() call status -1: %s", strerror(errno));
      }

      return ( (int)buf.msg_qnum );
}

int getmqcount_(int *mqid)
{
    return (getmqcount(*mqid));
}
