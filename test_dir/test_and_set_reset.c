/*
 * ROUTINE(S): test_and_set_reset
 *
 * DESCRIPTION:
 *    Provides functions to initialize, destroy, lock and unlock
 *    Posix mutex locks
 *
 * FUNCTIONS:
 *    test_and_set
 *    test_and_set_
 *    test_and_reset
 *    test_and_reset_
 *    posix_lock_init
 *    posix_lock_init_
 *    posix_lock_destroy
 *    posix_lock_destroy_
 *    test_lock
 *    test_lock_
 *    test_lock_block_flg
 *    test_lock_block_flg_
 *    set_lock_block_flg
 *    set_lock_block_flg_
 *    clear_lock_block_flg
 *    clear_lock_block_flg_
 *
 * HISTORY:
 *    02/16/01  Paul Taub       (Dot4) Initial coding
 *    02/27/01  Paul Taub       (Dot4) Added entries: test_lock & test_lock_
 *    07/31/01  Paul Taub       (Dot4) Using new posix_mlock_t struct & added
 *                                     test_lock_block_flg, set_lock_block_flg &
 *                                     clear_lock_block_flg functions
 *    03/02/11  GJ             tt60135 Remove block_flg related functions.
 *    11/12/14  gj             tt82153 Change test_and_reset/test_and_reset_
 *                                     to return void
 *
 */

/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  /* has process shared defined and include for types.h  */
#include <sys/types.h>
#include <errno.h>
#include <posix_mlock.h> 
#include <dlogdefs.h>
#include "proto_common.h" //tt82153


/*------------------------------------------------------------------------------
 *
 * ROUTINE(S): test_and_set test_and_set_
 *
 * INPUTS:
 *    Parameters:
 *       mptr      - posix lock structure
 *       bitno     - bit number to set (dummy)
 *    Variables:
 *       None (or) name of global variable - description
 *
 * OUTPUTS:
 *    Return Value:
 *      0          - OK
 *      EBUSY      - mutex already locked
 *      EINVAL     - un-initialized mutex lock
 *      EAGAIN     - max # of recursive locks exceeded for this mutex
 *      EOWNERDEAD - the last ownerof this mutex lock died while holding it
 *    Parameters:
 *       None
 *    Variables:
 *       None
 */

/*----------------------------------------------------------------------------*/

int test_and_set (posix_mlock_t *mptr) 
{
   int status;

    status = pthread_mutex_trylock (&(mptr->plock));

    return status;
}

/*----------------------------------------------------------------------------*/

int test_and_set_ (posix_mlock_t *mptr, const int *bitptr)
{
   int status;

    status = pthread_mutex_trylock (&(mptr->plock));

    return status;
}


/*------------------------------------------------------------------------------
 *
 * ROUTINE(S): test_and_reset test_and_reset_
 *
 * INPUTS:
 *    Parameters:
 *       mptr      - posix lock structure
 *       bitno     - bit number to set (dummy)
 *    Variables:
 *       None (or) name of global variable - description
 *
 * OUTPUTS:
 *    Return Value:
 *      0          - OK
 *      EBUSY      - mutex was locked
 *      EINVAL     - un-initialized mutex lock
 *      EAGAIN     - max # of recursive locks exceeded for this mutex
 *      EPERM      - current thread doesn't own this mutex
 *    Parameters:
 *       None
 *    Variables:
 *       None
 */

/*----------------------------------------------------------------------------*/

//int test_and_reset (posix_mlock_t *mptr) //tt82153
void test_and_reset( posix_mlock_t *mptr ) //tt82153
{
   int status, ret_status;

   /*----------------------------------------------*/
   /* First get present state of lock with trylock */
   /*----------------------------------------------*/
   ret_status = pthread_mutex_trylock (&(mptr->plock));
//   if (ret_status == 0) {
//     dLog(DLOG_DEBUG_16, "trylock successfully obtained the lock %d", ret_status);
//   }
//   else {
//     dLog(DLOG_DEBUG_16, "trylock error, return status is %d", ret_status);
//     perror("trylock error");
//   }
     
   /*----------------------------------------------*/
   /* Then attempt to unlock                       */
   // NOTE: status should always be 0 as the only realistic error conditions //tt82153
   //       are related to multi-threaded apps                               //tt82153
   /*----------------------------------------------*/
   status = pthread_mutex_unlock (&(mptr->plock));
//   if (status == 0) {
//     dLog(DLOG_DEBUG_16, "pthread_mutex_unlock successfully released the lock %d", status);
//   }
//   else {
//     dLog(DLOG_DEBUG_16, "unlock error, return status is %d", status);
//     perror("unlock error");
//   }

//   return ret_status; //tt82153
   return;              //tt82153
}



//int test_and_reset_ (posix_mlock_t *mptr, int *bitptr)
void test_and_reset_( posix_mlock_t *mptr, const int *bitptr )
{
//   return test_and_reset(mptr);
  test_and_reset( mptr );
  return;
}


/*------------------------------------------------------------------------------
 *
 * ROUTINE(S): posix_lock_init posix_lock_init_
 *
 * INPUTS:
 *    Parameters:
 *       mptr      - posix lock structure
 *    Variables:
 *       None (or) name of global variable - description
 *
 * OUTPUTS:
 *    Return Value:
 *      0          - OK
 *      EAGAIN     - system lacked resources (besides memory) to init a mutex
 *      EBUSY      - an attempt was made to re-initialize this active mutex
 *      EINVAL     - invalid mutex or attrib value specified
 *      ENOMEM     - insufficient memory exist to init the mutex or its attribs
 *      EPERM      - doesn't have privilege to initialize the mutex
 *    Parameters:
 *       mptr      - posix mutex lock structure
 *    Variables:
 *       None
 */

/*----------------------------------------------------------------------------*/

int posix_lock_init (posix_mlock_t *mptr) 
{
   int status;
   pthread_mutexattr_t mutex_shared_attr;


   /* First, initialize attributes object */
   if ((status = pthread_mutexattr_init (&mutex_shared_attr)) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_init: Can't init attributes");
      return status;
   }

   /* Next, set attribute to share miutex among process threads */
   if ((status = pthread_mutexattr_setpshared
                    (&mutex_shared_attr, PTHREAD_PROCESS_SHARED)) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_init: Can't set attributes");
      return status;
   }

   /* OK, initialize mutex object */
   if ((status = pthread_mutex_init (&(mptr->plock), &mutex_shared_attr)) != 0) {
      dLog(DLOG_MAJOR,"Init error");
      return status;  
   }

   return status;
}


/*----------------------------------------------------------------------------*/

int posix_lock_init_ (posix_mlock_t *mptr) 
{
   int status;
   pthread_mutexattr_t mutex_shared_attr;


   /* First, initialize attributes object */
   if ((status = pthread_mutexattr_init (&mutex_shared_attr)) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_init: Can't init attributes");
      return status;
   }

   /* Next, set attribute to share miutex among process threads */
   if ((status = pthread_mutexattr_setpshared
                    (&mutex_shared_attr, PTHREAD_PROCESS_SHARED)) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_init: Can't set attributes");
      return status;
   }

   /* OK, initialize mutex object */
   if ((status = pthread_mutex_init (&(mptr->plock), &mutex_shared_attr)) != 0) {
      dLog(DLOG_MAJOR,"Init error");
      return status;  
   }

   return status;
}


/*------------------------------------------------------------------------------
 *
 * ROUTINE(S): posix_lock_destroy posix_lock_destroy_
 *
 * INPUTS:
 *    Parameters:
 *       mptr      - posix lock structure
 *    Variables:
 *       None (or) name of global variable - description
 *
 * OUTPUTS:
 *    Return Value:
 *      0          - OK
 *      EBUSY      - can't destroy locked or referenced mutex 
 *      EINVAL     - un-initialized mutex lock
 *      EAGAIN     - max # of recursive locks exceeded for this mutex
 *      EOWNERDEAD - the last ownerof this mutex lock died while holding it
 *    Parameters:
 *       mptr      - posix mutex lock structure
 *    Variables:
 *       None
 */

/*----------------------------------------------------------------------------*/

int posix_lock_destroy (posix_mlock_t *mptr) 
{
   int status;


   /* First, try to make sure lock is free */
   if ((status = pthread_mutex_trylock (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Can't obtain lock mutex");
      return status;
   }

   /* OK, free  it */
   if ((status = pthread_mutex_unlock (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Can't unlock mutex");
      return status;
   }

   /* Now destroy it */
   if ((status = pthread_mutex_destroy (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Destroy error");
      return status;  
   }

   return status;
}

/*----------------------------------------------------------------------------*/

int posix_lock_destroy_ (posix_mlock_t *mptr) 
{
   int status;


   /* First, try to make sure lock is free */
   if ((status = pthread_mutex_trylock (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Can't obtain lock mutex");
      return status;
   }

   /* OK, free  it */
   if ((status = pthread_mutex_unlock (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Can't unlock mutex");
      return status;
   }

   /* Now destroy it */
   if ((status = pthread_mutex_destroy (&(mptr->plock))) != 0) {
      dLog(DLOG_MAJOR,"posix_lock_destroy: Destroy error");
      return status;  
   }

   return status;
}


/*------------------------------------------------------------------------------
 *
 * ROUTINE(S): test_lock test_lock_
 *
 * INPUTS:
 *    Parameters:
 *       mptr      - posix lock structure
 *    Variables:
 *       None (or) name of global variable - description
 *
 * OUTPUTS:
 *    Return Value:
 *      0          - mutex was unlocked
 *     -1          - mutex was locked
 *    Parameters:
 *       None
 *    Variables:
 *       None
 */

/*----------------------------------------------------------------------------*/

int test_lock (posix_mlock_t *mptr) 
{
   int status,
       ret_status;

   /* First get present state of lock */
   ret_status = pthread_mutex_trylock (&(mptr->plock));

   /* If necessary, return to previous state */
   if (!ret_status) {  /* was clear */
      status = pthread_mutex_unlock (&(mptr->plock));
      ret_status = 0;
   } else {
      ret_status = -1;
   }
   return ret_status;
}

/*----------------------------------------------------------------------------*/

int test_lock_ (posix_mlock_t *mptr)
{
   int status,
       ret_status;

   /* First get present state of lock */
   ret_status = pthread_mutex_trylock (&(mptr->plock));
   
   /* If necessary, return to previous state */
   if (!ret_status) {  /* was clear */
      status = pthread_mutex_unlock (&(mptr->plock));
      ret_status = 0;
   } else {
      ret_status = -1;
   }

   return ret_status;
}
