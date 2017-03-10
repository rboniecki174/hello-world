/*****************************************************************************/
/* monitor_rd.h     Status Values used by monitor_rd.c             R00-00.00 */
/*****************************************************************************/

#ifndef __MONITOR_RD_H__
#define __MONITOR_RD_H__

#ifndef YES
#define YES                      1            /* Yes, True etc.              */
#endif
#ifndef NO 
#define NO                       0            /* No, False etc.              */
#endif

#ifndef ON
#define ON                     YES
#endif
#ifndef OFF
#define OFF                     NO
#endif

#define ERR_NONE                 0            /* No Error                    */
#define ERR_TOUT                -1            /* Timeout/Retries Exhausted   */
#define ERR_RWPARTIAL           -2            /* Partial Packet Read/Write   */
#define ERR_NODATA              -3            /* No data present             */
#define ERR_INTERNAL            -4            /* Internal Error              */
#define ERR_SOCKET              -5            /* Socket error - closed etc.  */
#define ERR_PROTOCOL            -6            /* Protocol error              */
/*      ERR_ERRNO               >0 */         /* Positive values are errno   */

#endif

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

