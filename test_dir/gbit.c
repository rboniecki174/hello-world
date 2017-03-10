#include "stdio.h"

//**        BIT routines: gbset(), gbclr(), gbtest(), gbcmpl()
//**
//**  10 Jan 97         0.0     Pz      initial implementation
//**  20 Jan 97         Pz      modified to use DG bit routines
//**  20 Feb 97         Pz      added gbcmpl() routine 
//**   7 Mar 97         Pz      Modify bit number tobe 31-
//**  13 Mar 97         Pz      Treat first argument as array
//**                            (0:99) is arbitrary dimension
//**  20 Mar 97         Pz      Args as (Integer*2, Integer)
//**
//    2014-02-25       jeffs    78918 open ftoc changes 
//                              Trnanslate the fortran bit intrinsics to C.
//                              These routine are from mbit.f and gbit.f

//-------------------------------------------
int shorttest(short word, const int bitno)
{
   int bit;
   bit = (word >> bitno) & 1;
   return bit;
}

int btest(int word, const int bitno)
{
   int bit;
   bit = (word >> bitno) & 1;
   return bit;
}

int ibclr(int word, const int bitno)
{
     return (word &= ~(1<<bitno));
}

int ibset(int word, const int bitno)
{
     return (word |= (1<<bitno));
}
//-----------------------------------------
//--test up to 16 bits of a short array element.
//-----------------------------------------

void gbset(short word[], const int bitno)
{
     //int bit = 15-bitno%16;
     //printf ("****  15 minus (%d modulo 16) = %d\n",bitno, bit);
     word[bitno/16] = ibset(word[bitno/16],15-(bitno%16));
     return; 
}
void gbset_(short word4[], int *bitno)
{
   gbset(word4, *bitno);
}

void gbclr(short word[], const int bitno)
{
     word[bitno/16] = ibclr(word[bitno/16],15-(bitno%16));
     return; 
}
void gbclr_(short word4[], int *bitno)
{
     gbclr(word4, *bitno);
}

int  gbtest(short word[], const int bitno)
{
   if (shorttest(word[bitno/16],15-(bitno%16))) {
       return(1); 
   } else {     
       return(0); 
   }   
}
int  gbtest_(short word4[], int *bitno)
{
    return gbtest(word4, *bitno);
}

//-----------------------------------------
//test bytewise in an array of words.
// i.e. words are treated as 4 byte char arrays 
//-----------------------------------------

void gmset(void *arr, const int bitno)
{
     char *cbyte = arr;
     cbyte[bitno/8] = ibset(cbyte[bitno/8],7-(bitno%8));
     return;
}
void gmset_(int *word, int *bitno)
{
   gmset((void *)word, *bitno);
}

//-----------------------------------------
//-----------------------------------------
void gmclr(void *arr, const int bitno)
{
     char *cbyte = arr;
     cbyte[bitno/8] = ibclr(cbyte[bitno/8],7-(bitno%8));
     return;
}
void gmclr_(int *word, int *bitno)
{
     gmclr((void *)word, *bitno);
}

//-----------------------------------------
//-----------------------------------------
int  gmtest(void *arr, const int bitno)
{
   char *cbyte = arr;
   if (btest(cbyte[bitno/8],7-(bitno%8))) {
       return(1);
   } else {
       return(0);
   }
}
int  gmtest_(int *word, int *bitno)
{
    return gmtest((void *)word, *bitno);
}

//-----------------------------------------
// the following are test routines for the 
// routines above.
// The routines gbtestbits_() and gmtestbits_()
// can be put into a fortran main like ISSREL followed
// by an exit.  The libos32
// library can be then built with the old routines in mbit.f and gbit.f
// or the new routines all of which are in gbit.c.
// Build and run the fortran program both ways and compare the output.  
// Should be the same.
//-----------------------------------------

#if 0

//#include "stdio.h"
//#include "proto_common.h"
#define NUM 4

//----------------------
// test routines only
//----------------------
void gbtestall(unsigned short *num)
{
  int val;
  int bitno;
  for (bitno=0; bitno<NUM*16; bitno++) {
     val=gbtest_((short *)num, &bitno); 
     printf("    test >> for bit %d  result is %d\n", bitno, val);
  }
}

int gbtestbits(void)
{
     //int 
     unsigned short num[NUM];   
     int bitno;
     
     memset(num, 0, sizeof num);
     for (bitno=0; bitno<NUM*16; bitno++)
     {  
        gbset_(num, &bitno);
        printf("bit %d after set num=%04x %04x %04x %04x\n",bitno, num[0], num[1], num[2], num[3]);
        gbtestall(num);
        gbclr_(num, &bitno);
        printf("bit %d after clr num=%04x %04x %04x %04x\n",bitno, num[0], num[1], num[2], num[3]);
        printf("-----------------------------\n");
        printf("-----------------------------\n");
     }
     return 1;
}

int gbtestbits_(void)
{
   return gbtestbits();
}

void gmtestall(unsigned int *num)
{
  int val;
  int bitno;
  for (bitno=0; bitno<NUM*8; bitno++) {
     val=gmtest_((int *)num, &bitno); 
     printf("    test >> for bit %d  result is %d\n", bitno, val);
  }
}

int gmtestbits(void)
{
     unsigned int num[NUM]; 
     //unsigned short num[NUM];   
     int bitno;
     
     memset(num, 0, sizeof num);
     for (bitno=0; bitno<(NUM*32); bitno++)
     {  
        gmset_( (int *)num, &bitno);
        printf("bit %d after set num=%08x %08x %08x %08x n",bitno, num[0], num[1], num[2], num[3]);
        gmtestall(num);
        printf("-----------------------------\n");
        gmclr_((int *)num, &bitno);
        printf("bit %d after clr num=%08x %08x %08x %08x\n",bitno, num[0], num[1], num[2], num[3]);
        printf("-----------------------------\n");
        printf("-----------------------------\n");
     }
     return 1;
}

int gmtestbits_(void)
{
   return gmtestbits();
}

#endif
