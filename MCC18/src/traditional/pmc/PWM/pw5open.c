#include <p18Cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  OpenPWM5                                       *
*    Return Value:   void                                           *
*    Parameters:     period: PWM period                             *
*    Description:    This routine first resets the PWM registers    *
*                    to the POR state.  It then configures clock    *
*                    source.                                        *
********************************************************************/
void OpenPWM5( char period )
{
  CCP5CON=0b00001100;    //ccpxm3:ccpxm0 11xx=pwm mode
  if (T3CONbits.T3CCP2 == 0 && T3CONbits.T3CCP1 == 0)
  {
     T2CONbits.TMR2ON = 0;  // STOP TIMER2 registers to POR state
     PR2 = period;          // Set period
     TRISGbits.TRISG4=0;    //configure pin portg,4 an output
     T2CONbits.TMR2ON = 1;  // Turn on PWM5
  }
  else
  {
     T4CONbits.TMR4ON = 0;  // STOP TIMER4 registers to POR state
     PR4 = period;          // Set period
     TRISGbits.TRISG4=0;    //configure pin portg,4 an output
     T4CONbits.TMR4ON = 1;  // Turn on PWM4
  }
}
