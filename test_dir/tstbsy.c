//----------------------------------------------------------------------------------
//  File : tstbsy.c
//
//  Test for lock availability functions.
//
//----------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                               Modification History
//
//  Revision   When       Who    Why
//
//  tt60135    03-02-11   GJ     Standardize all protoypes that use locks to require
//                               type posix_mlock_t
//  tt82153    11-12-14   GJ     Change test_and_reset/test_and_reset_ to return void
//----------------------------------------------------------------------------------

#include <stdio.h>
#include "issdb.h"
#include "issrecord.h"
#include "dlogdefs.h"
#include <posix_mlock.h>
#include "proto_common.h"
#include "portability.h"           //60119

/*---------------------------------------------------------------------------------
* J. Strosberg 02/18/2010 TT 53189
* this algorithm will result in a x timeout after checking the lock about y times.
* target will be to check about every 20 milliseconds over a span of 125 milliseconds
* before giving up.  Needed this resolution for high performance systems and benchmarks. 
*---------------------------------------------------------------------------------*/

#define MAX_LOOP    100000  //  More than enough for any system (as of now)
#define TARGET_WAIT 125000  // microseconds

static void spin_awhile(int i);

void tstbsy_(const char *caller, posix_mlock_t *flag, const int *val)
{
   int i4val = *val;
   int status;
   status = tstbsy(caller, flag, i4val);
   // what to do with status? ftn ignores! 
}

int tstbsy(const char *caller, posix_mlock_t *flag, const int i4val) 
{
   int   i;
   int   i4isc;
   long long snap, elapsed;
   rcb_t *rptr;

   i4isc = (i4val>0) ? i4val : 0;   //if > 0 then it is a locked issue
   snap = GETHRTIME();           //60119
   for (i=0; i<MAX_LOOP; i++) {
     if (test_and_set(flag) != 0) {
       //-------------------------------
       // Test failed, no lock obtained
       //-------------------------------
       if (((i+1)%1000==0) && (i4isc>0)) {
           //------------------------------------------------------------------------
           // Only after lots of loops announce who is holding the lock (if isc > 0).
           //------------------------------------------------------------------------
           rptr = &issdb1_.issrcb[(i4isc)-1]; 
           dLog(DLOG_WARNING,"%s: WAITING %d retries on the lock held by <%.8s> isc=%d",
                caller,i, rptr->taskname, i4isc);
       }
     }
     else {
        //-----------------------------------
        // Test succeeded, lock was obtained
        //-----------------------------------
        if (i>1) { 
          //---------------------------------------------
          // only print if lock not obtained right away and tried 2nd time.
          //---------------------------------------------
          if (i4isc>0)
              dLog(DLOG_DEBUG_LOCK,"%s: GOT lock for isc=%d after %d tries (%lld usec)!", 
                   caller, i4isc, i, (GETHRTIME()-snap)/1000);      //60119
          else
              dLog(DLOG_DEBUG_LOCK,"%s: GOT lock after %d tries (%lld usec)!", 
                   caller, i, (GETHRTIME()-snap)/1000);             //60119
        }
        //-----------------------
        // return with success
        //-----------------------
        return(1);    
     }
     //--------------------------------------------------
     // decide whether to wait a while or give up trying
     //--------------------------------------------------
     elapsed = (GETHRTIME()-snap)/1000;                             //60119
     if (elapsed > TARGET_WAIT) {
       break;
     }
     else {
       spin_awhile(i);
     }
   }
   if (i4isc>0)
      dLog(DLOG_MAJOR,"%s: Retries Exhausted! TSTBSY Lock isc=%d released after %d attempts (%lld usec)." ,
            caller,i4isc,i,elapsed);
   else
      dLog(DLOG_MAJOR,"%s: Retries Exhausted! TSTBSY Lock released after %d attempts (%lld usec)." ,
            caller,i,elapsed);

   // now take the lock by force   
   //i=test_and_reset(flag); //tt71955
   test_and_reset(flag);     //tt82153
   i=test_and_set(flag);
   return(-1);
}

//-----------------------------------------------------------------------
// Do not optimize this routine since we want the overhead to be fixed. 
//-----------------------------------------------------------------------
int x_tstbsy;   // use global to prevent being optimized away!
#ifndef __linux__
#pragma opt 0 (spin_awhile)
#endif
static void spin_awhile(int i)
{ 
     int j;
     //x_tstbsy is external so is not optimized away.//
     for (x_tstbsy=0,j=0; j<5000; j++)
     {
        x_tstbsy+=((j*2+1-i)+(i*2));
     }
}
