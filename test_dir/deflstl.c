/****************************************************************************/
/*                                                                          */
/* deflstll.c   Create a locking circular list                     R00-00.00 */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Programing Notes:                                                        */
/*                                                                          */
/* This routine is a replacement for the list.f subroutine.                 */      
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Revision History:                                                        */
/*                                                                          */
/* Revision   Date         Whom      Why                                    */
/* =========  ===========  ========  ====================================== */
/* R00-00.00  15/Apr/1999  DRLJr     Replacement for list.f                 */
/* R00-02.00  20/Feb/2001  PT        Mods for Sparc posix locks             */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include "list.h"
#include <dlogdefs.h>

/*==========================================================================*/
/*                                                                          */
/*==========================================================================*/

int posix_lock_init (posix_mlock_t *lock);
static const char errmsg[] =                               /* PT - 02/20/01 */
  { " deflstll : Circular list size error.  Size of %d forced to 0.\n" };

/*==========================================================================*/
/* deflstl()  ("C" Interface)                                                */
/*==========================================================================*/

void deflstl(cirlistl * list, int      size, int      tout)
{
#ifdef posix     /* PT - 02/20/01 */
  int status;

  /* initialize the posix lock */
  if ((status = posix_lock_init (&list->lock)) != 0x00) {
    dLog(DLOG_MAJOR,"deflst: Lock initialization error");
    size       = 0L;
  }
#endif
  if((size < 1L) || (size > 32767)) {
    dLog(DLOG_MAJOR,errmsg, size);
    size       = 0L;
  }

/*  list->hwof0  = (unsigned short     ) 0L; */               /* PT - 02/20/01 */
/*  list->lock   = (unsigned short     ) (tout & 0x7FFF); */  /* PT - 02/20/01 */
  list->nslots = (unsigned short     ) size;
  list->nused  = (unsigned short     ) 0L;
  list->curtop = (unsigned short     ) 0L;
  list->nxtbot = (unsigned short     ) 0L;
  return;
}

/*==========================================================================*/
/* deflstl_()  (FORTRAN Interface)                                           */
/*==========================================================================*/

void deflstl_(cirlistl * list, int      * size, int      * tout)
{
  deflstl(list,(*size),(*tout));
  return;
}


/*==========================================================================*/
/* tstdeflst()                                                              */
/*==========================================================================*/

#ifdef MAIN

#ifdef dgux                                                  /* PT - 02/20/01 */
#define PRTHDR(x)                                                       \
        dLog(DLOG_STDLOG,"List : %5hu  %5hu  %5hu  %5hu  %5hu  %5hu",  \
                x->hwof0,x->lock,x->nslots,x->nused,x->curtop,x->nxtbot);
#endif
#ifdef posix                                                 /* PT - 02/20/01 */
#define PRTHDR(x)                                                       \
        dLog(DLOG_STDLOG,"List : %5hu  %5hu  %5hu  %5hu  %5hu",  \
                x->lock,x->nslots,x->nused,x->curtop,x->nxtbot);
#endif

int      main(int      argc, char ** argv)
{
  int          array[32];
  int          size;   
  int          tout;
  cirlistl   * list; 

  dLog(DLOG_STDLOG,"sizeof cirlistl is %d.\n",sizeof(cirlistl));

  list  = (cirlistl *) array;

  size  =  5L;  
  tout  = 1000L;
  deflstl_(list,&size,&tout);
  PRTHDR(list);

  size  = -5L;  
  tout  = 1024L;
  deflstl_(list,&size,&tout);
  PRTHDR(list);

  size  = 70000L; 
  tout  = 99999L;
  deflstl_(list,&size,&tout);
  PRTHDR(list);

  exit(0L);
}

#endif

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
