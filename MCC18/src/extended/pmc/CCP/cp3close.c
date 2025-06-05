#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:    CloseCapture3                                *
*    Return Value:     void                                         *
*    Parameters:       void                                         *
*    Description:      This routine disables the capture interrupt. *
********************************************************************/
void CloseCapture3(void)
{
   PIE3bits.CCP3IE = 0; // Disable the interrupt
   CCP3CON=0x00;        // Reset the CCP module
}
