#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

static int skip_empty_flag = 0;

void dumpbuf_(int *idx, unsigned char *obuff, char* str, char* fncstr);

void dumpbuf_condensed_(int *idx, unsigned char *obuff, char* str, char* fncstr)
{
      skip_empty_flag = 1;
      dumpbuf_(idx, obuff, str, fncstr);
}

void dumpbuf(int idx, unsigned char *obuff, char* str, char* fncstr)
{
   dumpbuf_(&idx,obuff, str, fncstr);
}

void dumpbuf_(int *idx, unsigned char *obuff, char* str, char* fncstr) 
{
     /* jeff strosberg's dumpit subroutine */
      int i = 0;
      int j, skipit;
      time_t timerec = time(NULL);
      char datebuf[26];
      char ch;
      memset(datebuf,0x0,26);
      strncpy((char *)datebuf,(const char *)ctime(&timerec),19);
      fprintf(stdout,"%s %s: %s idx=%d\n", datebuf, fncstr, str, *idx);
      while (i < *idx) {

         if (skip_empty_flag == 1)
         {
           skipit=1;
           if (i < (*idx +10))
           {
             for (j=0;j<10;j++)  
             {
                if ((obuff[i+j] != 0) ||        // this line and the next 
                   (obuff[i+j+10] != 0))  skipit=0;
             }
             if (skipit) {
                i=i+10;
                continue;
             } 
           }
         }

         fprintf(stdout,
            "****%04d: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X  : <",
            i, 
            obuff[i+0],obuff[i+1],obuff[i+2],obuff[i+3],obuff[i+4],
            obuff[i+5],obuff[i+6],obuff[i+7],obuff[i+8],obuff[i+9]
         );

         j = 0;
         while (j < 10) { 
           ch = obuff[i+j]; 
           if (isprint(ch)) {
             fprintf(stdout,"%c",ch);
           }
           else {
             fprintf(stdout,".");
           }
           j++;
         }
         fprintf(stdout,">\n");
         i=i+10;
      }
      skip_empty_flag = 0;
      fflush(stdout);
}

