/* $Id: pw1close.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  ClosePWM1                                      *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM1.                   *
********************************************************************/
void ClosePWM1(void)
{
  CCP1CON=0;            // Turn off PWM1
#if defined(__18F1220) || defined(__18F1320)
  TRISBbits.TRISB3=1;   //Return pin to its default input
#else
  TRISCbits.TRISC2=1;   //Return pin to its default input status from output
#endif
}
