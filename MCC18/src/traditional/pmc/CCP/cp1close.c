/* $Id: cp1close.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:   CloseCapture1                                 *
*    Return Value:    void                                          *
*    Parameters:      void                                          *
*    Description:     This routine disables the capture interrupt.  *
********************************************************************/
void CloseCapture1(void)
{
   PIE1bits.CCP1IE = 0;    // Disable the interrupt
   CCP1CON=0x00;           // Reset the CCP module
}
