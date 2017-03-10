/**********************************************************************/
/*                                                                    */
/* crtio.c & scrtio_                                                  */
/*                                                                    */
/**********************************************************************/

//#include "libos32.h"    //tt80093
#include "proto_common.h" //tt80093

void crtio_(iopcb * pcb, int      * func, int      * lu,   char * buff,
                         int      * size, int      * rand            )
{
  sysio_(pcb,func,lu,buff,size,rand);
  return;
}

void scrtio_(iopcb * pcb, int      * func, int      * lu,   char * buff,
                          int      * size, int      * rand            )
{
  sysio_(pcb,func,lu,buff,size,rand);
  return;
}
/**********************************************************************/
/*                                                                    */
/**********************************************************************/

