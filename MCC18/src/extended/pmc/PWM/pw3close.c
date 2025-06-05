#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  ClosePWM3                                      *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM3.                   *
********************************************************************/
void ClosePWM3(void)
{
  CCP3CON=0;            // Turn off PWM3
  TRISGbits.TRISG0=1;   //Return pin to its default input status from output
}
