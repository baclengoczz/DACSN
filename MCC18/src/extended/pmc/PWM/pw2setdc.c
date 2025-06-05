/* $Id: pw2setdc.c,v 1.2 2005/08/18 21:40:42 nairnj Exp $ */
#include <p18cxxx.h>
#include <pwm.h>

/********************************************************************
*    Function Name: SetDCPWM2                                       *
*    Return Value:  void                                            *
*    Parameters:    dutycycle: holds 10-bit duty cycle value        *
*    Description:   This routine writes the 10-bit value from       *
*                   dutycycle into the PWM1 duty cycle registers    *
*                   CCPR2L and CCP2CON.                             *
********************************************************************/
void SetDCPWM2(unsigned int dutycycle)
{
  union PWMDC DCycle;

  // Save the dutycycle value in the union
  DCycle.lpwm = dutycycle << 6;

  // Write the high byte into CCPR2L
  CCPR2L = DCycle.bpwm[1];

  // Write the low byte into CCP2CON5:4
  CCP2CON = (CCP2CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);
}
