/* $Id: t2open.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  OpenTimer2                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer2    *
*    Description:    This routine first resets the Timer2 regs      *
*                    to the POR state and then configures the       *
*                    interrupt and clock source.                    *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
void OpenTimer2(unsigned char config)
{

  T2CON = (0xfb & config);  // Set all configuration values, but
                            // don't start timer yet

  TMR2 = 0;                 // Clear Timer2
  PIR1bits.TMR2IF = 0;

  if(config & 0x80)         // Enable timer interrupts?
    PIE1bits.TMR2IE = 1;
  else
    PIE1bits.TMR2IE = 0;

  T2CONbits.TMR2ON = 1; // Turn on Timer2
}
