#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:    CloseCapture5                                *
*    Return Value:     void                                         *
*    Parameters:       void                                         *
*    Description:      This routine disables the capture interrupt. *
********************************************************************/
void CloseCapture5(void)
{
   PIE3bits.CCP5IE = 0; // Disable the interrupt
   CCP5CON=0x00;        // Reset the CCP module
}
