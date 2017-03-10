#include <sys/types.h>
#include <netinet/in.h>

/* Fortran interface to hton and ntoh routines */

unsigned long htonl_(unsigned long *hostlong)
{
    return(htonl(*hostlong));
}

unsigned short htons_(unsigned short *hostshort)
{
    return(htons(*hostshort));
}

unsigned long ntohl_(unsigned long *netlong)
{
    return(ntohl(*netlong));
}

unsigned short ntohs_(unsigned short *netshort)
{
    return(ntohs(*netshort));
}
