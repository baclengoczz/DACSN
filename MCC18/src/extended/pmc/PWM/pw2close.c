/* $Id: pw2close.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  ClosePWM2                                      *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM2.                   *
********************************************************************/
void ClosePWM2(void)
{
  CCP2CON=0;          // Disable PWM2
  TRISCbits.TRISC1=1; //Turn off PIN as an output to input--default state
}
