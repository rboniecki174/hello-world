/************************************************************************************/
/*                                                                                  */
/* helparg1.c   Help Discription of Run-File Argument                     R00-00.00 */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Purpose:                                                                         */
/*                                                                                  */
/* This file contains "C" definition of the routines that were located in the file  */
/* opendisplay.f.                                                                   */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Arguments:                                                                       */
/*                                                                                  */
/*   See the individual routines                                                    */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Programming Notes:                                                               */
/*                                                                                  */
/*  These routines are intended to be called from FORTRAN code or "C" code that     */
/*  acts like FORTRAN code.                                                         */
/*                                                                                  */
/* ================================================================================ */
/*                                                                                  */
/* Revision:                                                                        */
/*                                                                                  */
/* Revision   Who          When         Why                                         */
/* =========  ===========  ===========  =========================================== */
/* R00-00.00  DRLJr        06/Apr/1999  File created to replace opendisplay.f       */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>

#define  DOWRITE(x)  fprintf(stderr,x)

/*==================================================================================*/
/*                                                                                  */
/*==================================================================================*/

void helparg1_(void)
{
  DOWRITE("\n");
  DOWRITE(" Display/Program identifier Required ");
  DOWRITE("\n");
  DOWRITE("   Format: XDPnnn ");
  DOWRITE("\n");
  DOWRITE("    X   = L for Locally attached terminal ");
  DOWRITE("          T for TCP/IP attached display/terminal ");
  DOWRITE("            The use of client or server mode is  ");
  DOWRITE("            determined by the P (Program) option.");
  DOWRITE("          S for TCP/IP attached display/terminal ");
  DOWRITE("            using server mode.                   ");
  DOWRITE("          C for TCP/IP attached display/terminal ");
  DOWRITE("            using client mode.                   ");
  DOWRITE("\n");
  DOWRITE("            For the T, S and C option the program ");
  DOWRITE("            takes two additional arguments :      ");
  DOWRITE("              2nd argument is Hostname ");
  DOWRITE("              3rd argument is Port Number ");
  DOWRITE("\n");
  DOWRITE("    D   = The specific Desk (A, B, C etc.) ");
  DOWRITE("\n");
  DOWRITE("    P   = The program identification ");
  DOWRITE("            B for BRKSHRT                (TCP=Server)");
  DOWRITE("            S for SYSEXC                 (TCP=Server)");
  DOWRITE("            M for MDEQTRNS (Marpad)      (TCP=Client)");
  DOWRITE("            P for RPTHDL (Print Handlers)(TCP=Client)");
  DOWRITE("            N for others (i.e. N/A)      (TCP=Client)");
  DOWRITE("\n");
  DOWRITE("    nnn = Three digit terminal id number ");
  DOWRITE("\n");

  return;
}

/************************************************************************************/
/*                                                                                  */
/************************************************************************************/
