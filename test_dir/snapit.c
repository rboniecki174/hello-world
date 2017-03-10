#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <portability.h>           //60119
/*-------------------------------------------------------------------*/
/*  -------------              Source               ---------------- */
/*-------------------------------------------------------------------*/
/* this routine will return the "hrtime" clock tick in milliseconds  */
/*####################################################*/
/*####################################################*/
/*####################################################*/
/*####################################################*/
int snapit_(void)
{
   return( ( (int)((long long)GETHRTIME()/1000000) )% 10000000 ); //60119
}
long usnapit(void)    /* time in usec */
{
   return( ( (int)((long long)GETHRTIME()/1000) )% 10000000 );    //60119
}
long usnapit_(void)    /* time in usec */
{
   return( ( (int)((long long)GETHRTIME()/1000) )% 10000000 );    //60119
}

#if 0
main()
{
   int snap1, snap2;   
   printf("timestamp in msec = %d\n",snapit_());
   sleep(1);
   printf("timestamp in msec = %d\n",snapit_());

   printf("timestamp in msec = %lld\n",GETHRTIME())
   sleep(1);
   printf("timestamp in msec = %lld\n",GETHRTIME());

   printf("timestamp in usec = %08d\n",snapit_());
   sleep(1);
   printf("timestamp in usec = %08d\n",snapit_());
 
   snap1 = usnapit_();
   sleep(1);
   snap2 = usnapit_();
   printf("timestamp in usec = %08d\n",snap2-snap1);
   sleep(1);

}
#endif
