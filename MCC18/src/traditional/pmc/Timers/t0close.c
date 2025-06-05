/* $Id: t0close.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  CloseTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer0 interrupt.    *
********************************************************************/
void CloseTimer0(void)
{
  INTCONbits.TMR0IE = 0;  // Disable the Timer0 interrupt
  T0CON=0x7F;             //Stop TMR0
}
