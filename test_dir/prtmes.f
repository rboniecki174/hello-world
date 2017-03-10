C     PRTMES.FTN
C
C     REV        DATE          DESCRIPTION
C     ---       ------         -----------
C      0        04/15/83       INITIAL RELEASE
C      1        07/15/83       UPDATED FOR TRADE FILE
C
C     THIS SUBROUTINE PRINTS OUT ERROR MESSAGES FOR THE DATABASE
C ACCESS ROUTINES.
C
C     ARGUMENTS:
C
C     MESTYP:   MESSAGE TYPE
C        1   -   ILLEGAL WRITE OF CUSTOMER RECORD
C        2   -   UNLOCK OF CUSTOMER RECORD
C        3   -   ILLEGAL WRITE OF ISSUE RECORD
C        4   -   UNLOCK OF ISSUE RECORD
C        5   -   ILLEGAL UNLOCK OF CUSTOMER RECORD
C        6   -   ILLEGAL UNLOCK OF ISSUE RECORD
C        7   -   ILLEGAL WRITE OF TRADE RECORD
C        8   -   UNLOCK OF TRADE RECORD
C        9   -   ILLEGAL UNLOCK OF TRADE RECOD
C
C     RCB   -   INTEGER*4 ARRAY - PARTICULAR RCB IS PASSED
C     NEWTASK-  8 CHARACTERS IN A INTEGER*4 ARRAY
C                    (NAME OF CALLING TASK)
C     NUMBER -  ISSUE, CUSTOMER, OR TRADE  RECORD NUMBER
C-----------------------------------------------------------------

      SUBROUTINE PRTMES(MESTYP,RCB,NEWTASK,NUMBER4)
      IMPLICIT NONE
      CHARACTER*9 CFN
      PARAMETER(CFN='prtmes.f'//CHAR(0))

      INCLUDE 'dlogdefs.inc'

C
C     -------------------------------------------------------------------
C     DATA DECLARATIONS
C     -------------------------------------------------------------------
      INTEGER*4 RCB(*),NEWTASK(*),NEWT(2),OLDT(2)
      INTEGER*4 NUMBER4							!jeffs-16Nov2000
      CHARACTER*62 BUFF
      CHARACTER*8 BNEWT,BOLDT
      EQUIVALENCE (BNEWT,NEWT(1)),(BOLDT,OLDT(1))
      INTEGER*2  MESTYP,MESR,ASCII,MESR2  
      INTEGER*2  ITIME,TIME,UNLOCK       
      INTEGER*2  I,TEMP,TEMP1,HEX       

C     -------------------------------------------------------------------
C     DEFINE MESTYP TO RECORD TYPE (MESR - FIRST LINE, MESR2 -SECOND 
C     -------------------------------------------------------------------
      DIMENSION MESR(9),ASCII(3),MESR2(9)
      DATA MESR/1,1,2,2,1,2,3,3,3/,MESR2/2,2,1,1,2,1,3,3,3/
      CHARACTER*6 MEST1(3),CASCII,CTIME,MESTEMP
C     -------------------------------------------------------------------
C     TYPE OF OPERATION
C     -------------------------------------------------------------------
      CHARACTER*21 FTEMP,FTEMPS(2)
      DATA FTEMPS(1)/' ATTEMPTED WRITE OF  '/
      DATA FTEMPS(2)/' ATTEMPTED UNLOCK OF '/
      EQUIVALENCE (CASCII,ASCII(1))
      DATA MEST1/'CUST. ','ISSUE ','TRADE '/
      DIMENSION ITIME(3),TIME(3)
      CHARACTER*4 CHEXAR
      CHARACTER*1 CHEX(4)
      EQUIVALENCE (CHEX(1),CHEXAR)
C     -------------------------------------------------------------------
C     INTEGER TO HEX ASCII TABLE
C     -------------------------------------------------------------------
      CHARACTER*1 HEXTAB(16)
      DATA HEXTAB/'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'/
      EQUIVALENCE (CTIME,ITIME(1))
C
C     ------------------------------------------------------------
C     PRINT FIRST LINE OF MESSAGE
C     ------------------------------------------------------------
C     BLANK LINE BUFFER
      BUFF = ' '
C     COPY TASK NAME
      NEWT(1) = NEWTASK(1)
      NEWT(2) = NEWTASK(2)
C
C     ------------------------------------------------------------
C     CHECK FOR RECORD NOT PREVIOUSLY LOCKED
C     ------------------------------------------------------------
      UNLOCK = 0
      IF (RCB(1) .EQ. 0 ) THEN
         UNLOCK = 1
      ELSE
C        ---------------------
C        COPY OLD TASK NAME
C        ---------------------
         OLDT(1) = RCB(1)
         OLDT(2) = RCB(2)
      ENDIF
C     ------------------------------------------------------------
C     GET RECORD TYPE
C     ------------------------------------------------------------
      MESTEMP = MEST1(MESR(MESTYP))
C     ------------------------------------------------------------
C     CONVERT ISSUE OR CUSTOMER NUMBER TO ASCII
C     ------------------------------------------------------------
      CALL INTASC(ASCII,NUMBER4,INT2(4)) 
C     ------------------------------------------------------------
C     BUILD MESSAGE BASED ON LOCKING CONDITIONS AND MESTYP
C     ------------------------------------------------------------
      IF ((MESTYP.NE.2).AND.(MESTYP.NE.4.).AND.(MESTYP.NE.8))THEN
C        ------------------------------------------------------------
C        SET UP PROPER MESSAGE
C        ------------------------------------------------------------
         FTEMP = FTEMPS(1)
         IF((MESTYP.EQ.5 ).OR.(MESTYP.EQ.6).OR.(MESTYP.EQ.9)) THEN
             FTEMP = FTEMPS(2)
         ENDIF
         IF (UNLOCK .EQ. 0) THEN
          BUFF = '  '//BNEWT//FTEMP//MESTEMP//CASCII(1:4)//' LOCKED BY '//BOLDT
         ELSE
          BUFF = '  '//BNEWT//FTEMP//MESTEMP//CASCII(1:4)//' UNLOCKED'
         ENDIF
      ELSE
         BUFF = '  '//BNEWT//' UNLOCKED '//MESTEMP//CASCII(1:4)//' LOCKED BY '//BOLDT
      ENDIF
C     -------------------------------------------------------
C     ISSUE MESSAGE
C     -------------------------------------------------------
      CALL DLOGPF(DLOG_MAJOR,CFN,'%.62s'//ZZ, BUFF);
C
C     -------------------------------------------------------
C     PRINT SECOND LINE OF MESSAGE IF NECESSARY
C     -------------------------------------------------------
      IF (UNLOCK .EQ. 0) THEN
         BUFF = ' '
C       -------------------------------------------------------
C        FORM TIME
C       -------------------------------------------------------
         TIME(1) = RCB(3)/3600
         TIME(2) = (RCB(3)-TIME(1)*3600)/60
         TIME(3) = MOD(RCB(3),60)
C       -------------------------------------------------------
C        FORM ASCII TIME
C       -------------------------------------------------------
         DO I=1,3
           TEMP = TIME(I)
           CALL INTASC(TEMP1,INT(TEMP),INT2(2))                   !jefI4
           ITIME(I) = TEMP1
         ENDDO
C       -------------------------------------------------------
C       DO NOT BUILD UP OPERATION CODE, CUST/ISSUE # FOR TRADE
C       -------------------------------------------------------
        IF(MESTYP .LT. 7)THEN
C          GET OPERATION CODE IN HEX
           TEMP= RCB(4)						!jeffs-10Nov2000
C          FORM HEX DIGITS
           DO I=1,4
             HEX = IAND(X'F',ISHFT(TEMP,-(16-4*I)))
             CHEX(I) =CHAR(ICHAR( HEXTAB(HEX+1)))
           ENDDO
C          -------------------------------------------------------
C          EXTRACT CUST /ISSUE NUMBER
C          -------------------------------------------------------
           TEMP= RCB(5)						!jeffs-10Nov2000!jeffs-10Nov2000
           CALL INTASC(ASCII,INT(TEMP),INT2(3))                   !jefI4 
C          -------------------------------------------------------
C          FORM MESSAGE
C          -------------------------------------------------------
           MESTEMP = MEST1(MESR2(MESTYP))
           BUFF = '  LOCK TIME '//CTIME//', '//MESTEMP//' '//CASCII(1:3)//' , OPERATION '//CHEXAR
           CALL DLOGPF(DLOG_MINOR,CFN,'%.62s'//ZZ, BUFF);
        ENDIF
      ENDIF
      RETURN
      END
