/* $Id: pw2open.c,v 1.4 2005/02/21 18:41:52 nairnj Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  OpenPWM2                                       *
*    Return Value:   void                                           *
*    Parameters:     period: PWM period                             *
*    Description:    This routine first resets the PWM registers    *
*                    to the POR state.  It then configures clock    *
*                    source.                                        *
*    Notes:          The bit definitions for config can be found    *
*                    in the pwm.h file.                             *
********************************************************************/
void OpenPWM2( char period )
{
  CCP2CON=0b00001100;    //ccpxm3:ccpxm0 11xx=pwm mode
#if defined(__18F6520) || defined(__18F6620) || defined(__18F6720) || \
    defined(__18F8520) || defined(__18F8620) || defined(__18F8720) || \
    defined(__18F6525) || defined(__18F6621) || \
    defined(__18F8525) || defined(__18F8621) || \
    defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
    defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
    defined(__18F6527) || defined(__18F6622) || \
    defined(__18F6627) || defined(__18F6722) || \
    defined(__18F8527) || defined(__18F8622) || \
    defined(__18F8627) || defined(__18F8722)
  if (T3CONbits.T3CCP1 == 0)
  {
     T2CONbits.TMR2ON = 0;  // STOP TIMER2 registers to POR state
     PR2 = period;          // Set period
     TRISCbits.TRISC1=0;    //Set Pin Portc,1 to output
     T2CONbits.TMR2ON = 1;  // Turn on PWM2
  }
  else
  {
     T4CONbits.TMR4ON = 0;  // STOP TIMER4 registers to POR state
     PR4 = period;          // Set period
     TRISCbits.TRISC1=0;    //configure pin portc,1 an output
     T4CONbits.TMR4ON = 1;  // Turn on PWM2
  }
#else
  T2CONbits.TMR2ON = 0;  // STOP TIMER2 registers to POR state
  TRISCbits.TRISC1=0;    //Set Pin Portc,1 to output
  PR2 = period;          // Set period
  T2CONbits.TMR2ON = 1;  // Turn on PWM1
#endif
}
