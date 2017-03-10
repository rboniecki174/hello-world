//-----------------------------------------------------
//  2013Apr19   jeffs   69689: Remove warnings under -Wall 
//                      replace the initialization in the header.
//-----------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "process_asn.h"

void initialize_processasn(void)
{
   memset(&processasn,0x0,sizeof(processasn));
   //set the fields that are non zero
   memset(processasn.CALL_FileName,   0x20, sizeof (processasn.CALL_FileName));  
   memset(processasn.FTN_OPEN_Status, 0x20, sizeof (processasn.FTN_OPEN_Status));  
   memset(processasn.FTN_OPEN_Access, 0x20, sizeof (processasn.FTN_OPEN_Access));  
   memset(processasn.FTN_OPEN_Form,   0x20, sizeof (processasn.FTN_OPEN_Form));  
}
