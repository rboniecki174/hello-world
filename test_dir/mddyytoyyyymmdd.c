#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------
// This was moved from libdbsetiss  for tt47230
// tt69689 2013-01-17 jeffs removed warning for *(bp++) and *(bp+2)
//---------------------------------------------------------------------

int MddyyToYyyymmdd(char * issuedate, int * outdate);

void mddyy_to_yyyymmdd_(char * issuedate, int * outdate, int *status) 
{
   *status = MddyyToYyyymmdd(issuedate, outdate);
}

int MddyyToYyyymmdd(char * issuedate, int * outdate) 
{
   char *bp = issuedate;
   char tmpBuf[10];
   int  y, m, d;
   int      tmpdate;

   *outdate = 0;

   if (strlen(issuedate) < 5)
      return(-1);

   memset(tmpBuf, 0, sizeof(tmpBuf));
   switch (*bp)
   {
      case '1':
         m = 1;
         break;
      case '2':
         m = 2;
         break;
      case '3':
         m = 3;
         break;
      case '4':
         m = 4;
         break;
      case '5':
         m = 5;
         break;
      case '6':
         m = 6;
         break;
      case '7':
         m = 7;
         break;
      case '8':
         m = 8;
         break;
      case '9':
         m = 9;
         break;
      case 'O':
         m = 10;
         break;
      case 'N':
         m = 11;
         break;
      case 'D':
         m = 12;
         break;
      default:
         return(-1);
         break;
   } 
   bp++;
   strncpy(tmpBuf, bp, 2);
   d = atoi(tmpBuf);
   (bp+=2);

   if (atoi(bp) < 80)
      y = (atoi(bp) + 2000);
   else
      y = (atoi(bp) + 1900);

   tmpdate = (y * 10000) + (m * 100) + d;


   *outdate = tmpdate;
   return(0);
}

