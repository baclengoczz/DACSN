/* $Id: t0open.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  OpenTimer0                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer0    *
*    Description:    This routine resets the Timer0 regs to the     *
*                    POR state and configures the interrupt, clock  *
*                    source, edge and prescaler.                    *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
void OpenTimer0(unsigned char config)
{
  T0CON = (0x7f & config);  // Configure timer, but don't start it yet
  TMR0H = 0;                // Reset Timer0 to 0x0000
  TMR0L = 0;
  INTCONbits.T0IF = 0;      // Clear Timer0 overflow flag

  if(config&0x80)           // If interrupts enabled
    INTCONbits.T0IE = 1;    // Enable Timer0 overflow interrupt
  else
    INTCONbits.T0IE = 0;

  T0CONbits.TMR0ON = 1;
}
