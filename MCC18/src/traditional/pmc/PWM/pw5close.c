#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  ClosePWM5                                      *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM5.                   *
********************************************************************/
void ClosePWM5(void)
{
  CCP5CON=0;            // Turn off PWM5
  TRISGbits.TRISG4=1;   //Return pin to its default input status from output
}
