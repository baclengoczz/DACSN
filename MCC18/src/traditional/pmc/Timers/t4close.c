#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  CloseTimer4                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer4 and the       *
*                    interrupt.                                     *
********************************************************************/
void CloseTimer4(void)
{
  T4CONbits.TMR4ON = 0;  // Disable Timer4
  PIE3bits.TMR4IE = 0;   // Disable Timer4 overflow interrupts
}
