//------------------------------------------------------------------------------------
// isleap.c   
//
//*   Determine if the year is a leap year (Leapyear)                        */
//*                                                                          */
//*   This routine will check the specified year to detirmine if the year is */
//*   a leap year.                                                           */
//*                                                                          */
//*   year  - This is the year to be checked.  The value starts at 1800.     */
//*                                                                          */
//  2013Nov19   jeffs  tt 78918 Conversion of isleap.f using Don Laster's missues/Leapyear()
//------------------------------------------------------------------------------------

int isleap(int year)
{
   if (( year % 4 ) == 0L) {
      return(1);
   }
   else {
      return(0);
   }
}

int isleap_(int *year)
{
   return isleap(*year);
}


//      INTEGER*4 FUNCTION ISLEAP(YEAR)
//      IMPLICIT NONE
//      INTEGER*4 YEAR
//
//      IF (MOD ( YEAR, 4 ) .EQ. 0) THEN
//         ISLEAP = 1
//      ELSE
//         ISLEAP = 0
//      ENDIF
//      RETURN
//      END
