********************************************************************
**                                                                **
**	LIST header                                               **
**                                                                **
**  18 Jan 1997 0.0   Pz     initial implementation, in Fortran   **
**  15 Apr 1997 0.1   DRLJr  Added List Access features and some  **
**                           cleanup of the header.  Maximum      **
**                           number of slots reduced from 65535   **
**                           to 32767 to allow for lock bit.      **
**  02 May 1997 0.2   clb    define list.slot array to have a     **
**                           begining subscript of zero (0).      **
**                                                                **
********************************************************************
**
**          Format of the Circular List
**
**       +-0-----+-------+-2-----+-------+
**       |   nslots      |   used        |
**       +-4-----+-------+-6-----+-------+
**       |   current top |   next bottom |
**       +-8-----+-------+-------+-------+
**       |           slot 0              |
**       +-C-----+-------+-------+-------+
**       |                               |
**       *            *  *  *            *
**       |                               |
**       +4n+4---+-------+-------+-------+
**       |           slot n-1            |
**       +4n+8---+-------+-------+-------+
**
**  General Notes On Circular List structure
**
**       Each of the four header fields is a halfword (2 bytes)
**    in size.  The high order bit of the nslots field is used
**    as a lock bit.  This restricts the number of slots in the
**    list to 32767 slots.
**
**       Each of the slots is a fullword in size (4 bytes) and
**    is added to the list as a fullword.  When the fullword 
**    contains "structured" data it should be combined into a
**    fullword and then placed on the list.  When removed from the
**    list it should be handled in the reverse order.
**
**
      STRUCTURE /LIST/              ! Add/Remove Top/Bottom LIST structure
	  INTEGER*2   size          ! Size of the list and lock bit
          INTEGER*2   used          ! Number of used slots
          INTEGER*2   ctop          ! Current top of the list
          INTEGER*2   nbot          ! Next bottom of the list
          INTEGER*4   slot(0:32767) ! maximum number of slots  ! clb
      END STRUCTURE

C     Add STATUS values
      INTEGER*4  ADDED,     WASFULL
      PARAMETER  ADDED = 0, WASFULL = 1 

C     Remove STATUS values
      INTEGER*4  NOWEMPTY,     NOTEMPTY,     WASEMPTY
      PARAMETER  NOWEMPTY = 0, NOTEMPTY = 1, WASEMPTY = 2
C
C     Lock Error Status Flag and Lock Values
      INTEGER*4  LOCK_TOUT, LIST_UNITS, LIST_DELAY, LIST_LOOP
C
      PARAMETER  LOCK_TOUT  =   4 ! Proper value may be 8 (DRLJr)
      PARAMETER  LIST_UNITS =   1 ! Millisecond Delay
      PARAMETER  LIST_DELAY =  50 ! Number of Millseconds
      PARAMETER  LIST_LOOP  =  20 ! Loop length for 1 second
C
      INTEGER*4  LOCK_OVERRIDE   ! Override lock under condition 1
C     PARAMETER (LOCK_OVERRIDE= 0 )
      PARAMETER (LOCK_OVERRIDE= 1 )
C
********************************************************************
**                                                                **
********************************************************************

