/* $Id: ew1setdc.c,v 1.2 2005/08/18 21:40:42 nairnj Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name:  SetDCEPWM1                                     *
*    Return Value:   void                                           *
*    Parameters:     dutycycle: holds 10-bit duty cycle value       *
*    Description:    This routine writes the 10-bit value from      *
*                    dutycycle into the PWM1 duty cycle registers   *
*                    ECCPR1L and ECCP1CON.                          *
********************************************************************/
void SetDCEPWM1(unsigned int dutycycle)
{
  union PWMDC DCycle;

  // Save the dutycycle value in the union
  DCycle.lpwm = dutycycle << 6;

  // Write the high byte into ECCPR1L
  ECCPR1L = DCycle.bpwm[1];

  // Write the low byte into ECCP1CON5:4
  ECCP1CON = (ECCP1CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);
}
