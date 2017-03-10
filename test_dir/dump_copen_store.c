/*==================================================================================*/
/* dump_copen_store()                                                                    */
/*==================================================================================*/
//initialize the local-global lu storage for this process
// 2014-01-10       jeffs    78918 new file for debug of conversion of ftn open to C

#include <stdio.h>
#include <string.h>
#include "private_data.h"

void dump_copen_store(void)
{
  int lu;
  for (lu=0; lu<=255; lu++) {
    if(( fnum_store[lu] != -1) || (private_data.lu_to_fd[lu]!=-1)) {
      fprintf(stderr,"fnum_store[%d]=%d   private_data.lu_to_fd[%d] = %d\n",
        lu, fnum_store[lu], lu, private_data.lu_to_fd[lu]);
      }
  }
}
