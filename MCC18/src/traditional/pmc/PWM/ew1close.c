/* $Id: ew1close.c,v 1.1 2004/02/10 22:00:25 sealep Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  CloseEPWM1                                     *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine turns off PWM1.                   *
********************************************************************/
void CloseEPWM1(void)
{
  ECCP1CON=0;           // Turn off PWM1
  TRISDbits.TRISD4=1;   //Return pin to its default input status from output
}
