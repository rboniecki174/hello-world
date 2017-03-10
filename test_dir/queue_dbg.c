/************************************************************************************/
/*                                                                                  */
/* queue_dbg.c     Debug QUEUE Parameter Enable/State Support             R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        30/Mar/1999  Moved from queue.c                          */
/*                                                                                  */
/************************************************************************************/

/* Note: ECHO_QUEUE_PARMS is referenced in queue.c and enable.c to allow for debug  */
/*       of the queue parameter data sent between task.  The routine                */
/*       dbg_queue_write_() is called from gb1_init.f to enable the debugging       */

int        ECHO_QUEUE_PARMS = { 0 };

void dbg_queue_write_(int      * flag)
{
/*fprintf(stderr,"DEBUG...\n");*/
  ECHO_QUEUE_PARMS = (*flag);
  return;
}

int      dbg_queue_state_(void)
{
  return(ECHO_QUEUE_PARMS);
}


/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
