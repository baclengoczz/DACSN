#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  ClosePWM4                                      *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM4.                   *
********************************************************************/
void ClosePWM4(void)
{
  CCP4CON=0;            // Turn off PWM4
  TRISGbits.TRISG3=1;   //Return pin to its default input status from output
}
