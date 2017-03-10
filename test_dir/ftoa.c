//----------------------------------------------------------------------------------
// File :ftoa.c
//
// Given a real binary number(+ or -), return its ASCII representation.
//
// Function: snprintf 
//
//    I/P: fvalue      - ptr to double (float) typed value
//         cvalue_size - byte size of the char array that the ASCII value will be stored
//
//    O/P: cvalue - char array that will have the ASCII representation of the real value
//                  If fraction, a leading zero will be place to the left of the decimal
//                  point. Ex: 0.01, -0.007 etc.
//
//    RET: none
//----------------------------------------------------------------------------------
//                               Modification History
//  What       When       Who    Why
//  tt68061    03-27-12   GJ     No "-" sign from trade result for neg yield
//  tt69066    04-24-12   Jeffs  Problem with rounding (102.2499 instead of 102.25)
//                               use snprintf 
//----------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int snprintf(char *str, size_t size, const char *format, ...);

void ftoa( double fvalue, int cvalue_size, char * cvalue )
{
  int num;
  num = snprintf(cvalue, cvalue_size,"%lf", fvalue);
}

// Fortran interface
void ftoa_( double * fvalue, int * cvalue_size, char * cvalue )
{
  ftoa( *fvalue, *cvalue_size, cvalue );
  return;
}
