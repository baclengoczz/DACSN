/* $Id: ew1open.c,v 1.1 2004/02/10 22:00:28 sealep Exp $ */
#include <p18Cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  OpenEPWM1                                      *
*    Return Value:   void                                           *
*    Parameters:     period: PWM period                             *
*    Description:    This routine first resets the PWM registers    *
*                    to the POR state.  It then configures clock    *
*                    source.                                        *
********************************************************************/
void OpenEPWM1( char period )
{

  ECCP1CON=0b00001100;    //ccpxm3:ccpxm0 11xx=pwm mode

  T2CONbits.TMR2ON = 0;  // STOP TIMER2 registers to POR state
  PR2 = period;          // Set period
  TRISDbits.TRISD4=0;    //configure pin portc,2 an output
  T2CONbits.TMR2ON = 1;  // Turn on PWM1
}
