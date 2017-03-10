C This supplies the fortran 77 iargc and getarg function for Fortran 95 (gnu gfortran).

      INTEGER*4 FUNCTION IARGC() 
        IARGC = COMMAND_ARGUMENT_COUNT()
        RETURN 
      END

      SUBROUTINE GETARG(NUMB, CBUF)
      IMPLICIT NONE
      INTEGER*4 NUMB
      CHARACTER(*) CBUF
      CALL GET_COMMAND_ARGUMENT(NUMB, CBUF)
      RETURN
      END
