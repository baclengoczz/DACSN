/* $Id: t3open.c,v 1.2 2004/02/10 21:58:04 sealep Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  OpenTimer3                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer3    *
*    Description:    This routine first resets the Timer3 regs      *
*                    to the POR state and then configures the       *
*                    interrupt, clock source.                       *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
*                                                                   *
*                    The MPLAB CXX Reference Guide says that        *
*                    OpenTimer1(config) can select Timer 1 as the   *
*                    source for the CCPs.  However, this function   *
*                    does not write the necessary values to bits 6  *
*                    and 3 of T3CON.  Instead, the function         *
*                    incorrectly affects bit 3 of T1CON.            *
*                                                                   *
*                    Because the power-on default is timer1 source  *
*                    for both CCPs, we do not need to even have     *
*                    these values.  Should a user want to change    *
*                    this value, the user would need select the     *
*                    appropriate value when calling OpenTimer3.     *
*                                                                   *
********************************************************************/
void OpenTimer3(unsigned char config)
{
  T3CON = (0x7e & config);  // Set everything except 8/16 mode, and
                            // don't start timer yet

  TMR3H=0;          //Clear timer3-related registers
  TMR3L=0;
  PIR2bits.TMR3IF = 0;

  if(config & 0x80)   // Interrupt on/off
    PIE2bits.TMR3IE = 1;
  else
    PIE2bits.TMR3IE = 0;

  if(config & 0x02)
    T1CONbits.T1OSCEN = 1;  // Enable Timer 1 Oscillator enable
                            // (200KHz max freq)

  if(config & ~T3_8BIT_RW) // Select between 8-bit and 16-bit modes
  {
    T3CONbits.RD16 = 1;
  }
  else
  {
    T3CONbits.RD16 = 0;
  }

  T3CONbits.TMR3ON = 1;   // Turn on Timer3
}

