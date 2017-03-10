/* R00-00.00  jeffsun      13/Feb/2001  Port to Sun. Add conditional ushort definition.   */

#ifndef F7LIB
#define F7LIB
#include <sys/types.h>

#ifndef _BOFF_
#define _BOFF_(p, n) (((uchar_t *)(p))[(n)/8U])
#define _BITT_(n) (0x80 >> ((n) % 8U))
#define SBT(p, n) (_BOFF_(p,n) |=  _BITT_(n))
#define RBT(p, n) (_BOFF_(p,n) &= ~_BITT_(n))
#define TBT(p, n) (_BOFF_(p,n) &   _BITT_(n))
#define CBT(p, n) (_BOFF_(p,n) ^=  _BITT_(n))
#endif

#define NOWEMPTY 0
#define GOODADD  0
#define NOTEMPTY 1
#define LISTFULL 1
#define EMPTY	 2

#define ATL 1
#define RTL 2
#define ABL 3
#define RBL 4

//#if defined(__sparc) || defined(__i386)
//typedef unsigned short     ushort ;     /*jeffsun-02/08/01*/
//#endif

struct listhdr {
	ushort	total;
	ushort	used;
	ushort	ctop;
	ushort	nbot;
};

union	bytework {
	int	intval;
	char	byte[4];
};

#endif
