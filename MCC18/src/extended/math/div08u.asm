; RCS Header $Id: div08u.asm,v 1.2 2006/03/06 21:38:32 nairnj Exp $
; $Revision: 1.2 $

          include <P18CXXX.INC>         ; general Golden Gate definitions
          include <CMATH18.INC>         ; Math library definitions

;-------------------------------------------------------------------------
;
; 8-bit unsigned integer division / modulus
;
; Prototype:  unsigned char __div08u (unsigned char A, unsigned char B);
;             unsigned char __mod08u (unsigned char A, unsigned char B);
;
; Stack after setup:
;  FSR1 --> [6] Unused slot          INDF1 is used as a counter
;         > [5] Previous FSR2H
;         > [4] Previous FSR2L
;         > [3] Temporary location 2 (FLAGS
;                                     7:SIGN
;                                     1:Early exit
;                                     0:Division)
;         > [2] Temporary location 1 (REM)
;         > [1] Parameter A byte 0
;  FSR2 --> [0] Parameter B byte 0
;
; Result:     8-bit integer located on the stack (A/B or A%B)
;  FSR1 --> Unused slot
;         > Return value byte 0
;
;*** Optimized: Dec. 21, 2000
;***        by: Daniel R. Madill, Quanser Consulting Inc.
;***       for: Saved (for the worst case) at least 4*8 = 32 instruction cycles
;***            over the code supplied with MCC18 v1.00.12
;
;-------------------------------------------------------------------------

PROG      CODE

SETUPSTK
          ; Allocate temporary locations
          ADDFSR    1, 2

          ; Save current value of FSR2
          MOVFF     FSR2L, POSTINC1
          MOVFF     FSR2H, POSTINC1

          ; Set up FSR2 to point to the first parameter on the stack.
          ; The 6 is 2 bytes of parameters plus 2 byte for temporaries
          ; plus 2 bytes for the previous value of FSR2
          MOVFF     FSR1L, FSR2L
          MOVFF     FSR1H, FSR2H
          SUBFSR    2, 6

          CLRF      [3]
          RETURN    0

__div08u
          GLOBAL    __div08u

          RCALL     SETUPSTK
          BSF       [3], LSB
          BRA       divmod08u

__mod08u
          GLOBAL    __mod08u

          RCALL     SETUPSTK
          BRA       divmod08u

_early08uexit
          GLOBAL    _early08uexit

          BSF       [3], 1

divmod08u
          CLRF      [2]                 ; REM = 0

          MOVLW     8                   ; INDF1 = 8
          MOVWF     INDF1

          BCF       STATUS, LSB         ; clear the carry

loop
          ; Carry is always clear at this point.
          RLCF      [1], F              ; A <<= 1

          RLCF      [2], F              ; REM = (REM << 1) | (A >> 16);

          MOVF      [0], W              ; if (REM >= B)
          SUBWF     [2], W   
          BNC       endloop             ; {
          MOVWF     [2]                 ;   REM -= B;

          ; Carry is cleared by INCF since overflow is not possible
          INCF      [1], F              ;   ++A;

endloop                                 ; }
          DECFSZ    INDF1, F            ; does not affect carry
          BRA       loop

          ; If called from signed routine, want to exit without
          ; restoring the stack.
          BTFSC     [3], 1
          RETURN    0

RESTORE
          BTFSC     [3], LSB
          MOVSS     [1], [0]            ; Doing division.  Return quotient.
          BTFSS     [3], LSB
          MOVSS     [2], [0]            ; Doing modulus.  Return remainder.

          ; Restore FSR2
          MOVF      POSTDEC1, 1, 0
          MOVFF     POSTDEC1, FSR2H
          MOVFF     INDF1, FSR2L

          ; Deallocate parameter and temporary stack locations.
          ; NOTE: Return value location is not deallocated here.
          SUBFSR    1, 3

          RETURN    0

          END
