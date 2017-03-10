/*****************************************************************************
 * 27-Sep-2005 John Lindberg
 * 2007/11/07 jl new default file /production/integrate/cfg/dlog_fortran.cfg
 * ---------------------------------------------------------------------------
 * Copyright (C) 2004   ICAP Corporation. All rights reserved.
 *
 * The software and information contained herein are proprietary to, and
 * comprise valuable trade secrets of, ICAP Corp., which intends to
 * preserve as trade secrets such software and information. This software
 * is furnished pursuant to a written license or trial agreement and
 * may be used, copied, transmitted, and stored only in accordance with
 * the terms of such license and with the inclusion of the above
 * copyright notice. This software and information or any other copies
 * thereof may not be provided or otherwise made available to any other
 * person.
 ****************************************************************************/
/******************************************************************************
*
*TITLE
*   dlogpf.c
*
*Synopsis : Fortran wrapper for dLog.
*
*           For the FORTRAN/C string interchange to work all strings passed
*           has to be NULL terminated, i.e.  BUFF = 'Tick Tack'//char(0)
*           The second parameter to DLOGPRINTF_F is format string in C-notation.
*           E.G. CALL DLOGPRINTF_F(DLOG_STDERR, '%s %d %f'//char(0), string, int float)
*           any combination of datatypes is valid and basically any numbers of params.
*           However number of entities in the format statement has to match the number
*           fields passed.
*           It is assumed the config file is in a file opened in RUN-files.
*
*Usage:
*        INCLUDE 'dlogdefs.inc'      ! All codes and prototypes
*        .
*        .
*        .
*        .
*        CHARACTER*20 STR 
*        STR = 'STRING'//_Z
*        C run file opened
*
*        CALL DLOG_SETUP()
*        .
*        .
*
*        CALL DLOGPRINTF_F(DLOG_STDERR, 'func_name', '%s', STR)
*
*        .
*        .
*        .
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <libLog.h>
#include <stdarg.h>
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

//******************************************************************************
//
// Function:    dlogON(int logType)
//
// Description: Returns a state of specified tag
//
//******************************************************************************
int dlogon_f_(short      * logType)
{
   int loc_type;
   loc_type = (int) *logType;
   return dLogON(loc_type);
}

//******************************************************************************
//
// Function:    dlogprintf_f()
//
// Description: calls the lower level libOAM function dLogPrintf 
//
//******************************************************************************
//void dlogprintf_f_(short      * logType, const char *pszMask,...)
void dlogprintf_f_(short      * logType, char * call_func, const char *pszMask,...)
{
   char caMsg[1024];
   va_list ap;


   va_start(ap, pszMask);
   if (dLogTypes[*logType].state) {

      vsnprintf(caMsg, sizeof(caMsg), pszMask, ap);

      //dLog(*logType, "%s", caMsg);
      //_dLogPrintf(*logType, "FN", 1, __FUNCTION__, " ", "%s", caMsg);
      //dLogPrintf(*logType, call_func, "%s", caMsg);
      _dLogPrintf(*logType, " ", 0, call_func, " ", "%s", caMsg);
   }
   va_end(ap);
}
/*****THIS IS A DUP OF THE ABOVE FUNCTION JUST WITH SHORT NAME*****************/
//******************************************************************************
//
// Function:    dlogpf_()
//
// Description: calls the lower level libOAM function dLogPrintf 
//
//******************************************************************************
void dlogpf_(short      * logType, char * call_func, const char *pszMask,...)
{
   char caMsg[1024];
   va_list ap;

   va_start(ap, pszMask);
   if (dLogTypes[*logType].state) {

      vsnprintf(caMsg, sizeof(caMsg), pszMask, ap);

      //dLog(*logType, "%s", caMsg);
      //_dLogPrintf(*logType, "FN", 1, __FUNCTION__, " ", "%s", caMsg);
      //dLogPrintf(*logType, call_func, "%s", caMsg);
      _dLogPrintf(*logType, " ", 0, call_func, " ", "%s", caMsg);
   }
   va_end(ap);
}
