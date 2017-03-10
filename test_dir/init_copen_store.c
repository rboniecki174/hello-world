/*==================================================================================*/
/* initlustore()                                                                    */
/*==================================================================================*/
//initialize the local-global lu storage for this process
// 2014-01-10       jeffs    78919 convert ftn open to C

#include <stdio.h>
#include <string.h>
#include "private_data.h"

void init_copen_store(void)
{
// init to -1 indicating unopen.
  int lu;
  for (lu=0; lu<=255; lu++) {
    fnum_store[lu]=-1;
    recl_store[lu]=-1;
  }

// set up for LU 0, 5, 6 for stderr, stdin, stdout to emulate fortran
  fnum_store[0] = 2;    //assign lu 0 to fd 2  for stderr;
  fnum_store[5] = 0;    //assign lu 5 to fd 0  for stdin;
  fnum_store[6] = 1;    //assign lu 6 to fd 1  for stdout;
}
