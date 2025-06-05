/* $Id: cp2close.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:    CloseCapture2                                *
*    Return Value:     void                                         *
*    Parameters:       void                                         *
*    Description:      This routine disables the capture interrupt. *
********************************************************************/
void CloseCapture2(void)
{
   PIE2bits.CCP2IE = 0; // Disable the interrupt
   CCP2CON=0x00;        // Reset the CCP module
}
