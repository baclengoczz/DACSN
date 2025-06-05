/* $Id: t3close.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  CloseTimer3                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer3 and the       *
*                    interrupt.                                     *
********************************************************************/
void CloseTimer3(void)
{
  T3CONbits.TMR3ON = 0;  // Disable Timer3
  PIE2bits.TMR3IE = 0;   // Disable Timer3 overflow interrupts
}
