
// emulate the fortran itime function
// makefile should not build this on Sun

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

void itime_( int* tarr )
{
   struct timeval tv;
   struct tm     *tm;
   gettimeofday(&tv, NULL);
   tm = localtime(&tv.tv_sec);   
   tarr[0] = tm->tm_hour;
   tarr[1] = tm->tm_min;
   tarr[2] = tm->tm_sec;
}
