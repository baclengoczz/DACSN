/* $Id: ep1close.c,v 1.1 2004/02/10 22:06:54 sealep Exp $ */
#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:   CloseECapture1                                *
*    Return Value:    void                                          *
*    Parameters:      void                                          *
*    Description:     This routine disables the capture interrupt.  *
********************************************************************/
void CloseECapture1(void)
{
   PIE2bits.ECCP1IE = 0;    // Disable the interrupt
   ECCP1CON=0x00;           // Reset the CCP module
}
