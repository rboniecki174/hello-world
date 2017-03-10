// assign_cpu.c
//
// Note:
//   Cpu numbering is 0 relative.
//   The mask represents the cpu's that the process is allowed to run on.
//       So a mask of 8 represents cpu 3  :  00001000 
//       A mask of 7u represents cpu's 0 1 & 2 : 00000111
//   Return values:
//      -1  system call failure 
//       0  Successful assign
//       1  CPU not in the mask 
//
// From the man page
//       int sched_setaffinity(pid_t pid, unsigned int cpusetsize,
//                             cpu_set_t *mask);
//
//       int sched_getaffinity(pid_t pid, unsigned int cpusetsize,
//                             cpu_set_t *mask);
//
//       void CPU_CLR(int cpu, cpu_set_t *set);
//       int CPU_ISSET(int cpu, cpu_set_t *set);
//       void CPU_SET(int cpu, cpu_set_t *set);
//       void CPU_ZERO(cpu_set_t *set);
//
// 2014Aug05    jeffs   tt80903  Mechanism to pin a process to a cpu
//


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sched.h>
#ifdef __linux__
#include <linux/unistd.h>
#endif
#include <dlogdefs.h>
//Local Prototypes
static void list_cpu(unsigned long int mask, char *desc);

//----------------------------
//----------------------------
//----------------------------
int assign_cpu(const int cpu)
{
#ifdef __linux__
  int ret = 0;
  cpu_set_t my_set;           /* Define your cpu_set bit mask. */
  unsigned long *my_set_ptr = (unsigned long *)&my_set;       /* for printf warning */

  ret = sched_getaffinity(0, sizeof(cpu_set_t), &my_set);
  //dLog(DLOG_WARNING,"sched_getaffinity returns the current mask: %08lx", *my_set_ptr);
  list_cpu(*my_set_ptr,"Initial CPU Mask");

  errno = 0;
  dLog(DLOG_WARNING, "Information: Setting affinity to CPU core %d",cpu);
//-------------------------------
// set up the desired cpu mask
//-------------------------------
  CPU_ZERO(&my_set);          /* Initialize it all to 0, i.e. no CPUs selected. */
  CPU_SET(cpu, &my_set);      /* set the bit that represents core desired. */

  ret = sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of tihs process to */
  if (ret == -1) {
    dLog(DLOG_MINOR," Failure in sched_setaffinity for cpu %d.  Errno=%d", cpu, errno);
    return -1;
  }
  list_cpu(*my_set_ptr,"New CPU Mask");
  if (*my_set_ptr != (1 << cpu)) {       
     dLog(DLOG_MINOR," Specified cpu %d not set.", cpu);   //for some reason...should not happen
     return 1; 
  }
  else {
     dLog(DLOG_WARNING, "Information: The process is now assigned to core %d", cpu);
     return 0;
  }
#endif
}

//----------------------------
//----------------------------
//----------------------------
static void list_cpu(unsigned long int mask, char *desc)
{
    int i;
    FILE *dlogfp = dLogTypes[DLOG_WARNING].fd;
    fprintf(dlogfp, "%s: <",desc);
    for(i=8*sizeof(mask)-1; i>=0; i--) {
        if (mask & (1 << i)) {
           fprintf(dlogfp, " %d", i);
        }
    }
    fprintf(dlogfp, " >\n");
}



//int main(int argc, char * argv[])
//{
//  int ret;
//  assign_cpu(1);
//  printf("done\n");
//  return 1;
//}


