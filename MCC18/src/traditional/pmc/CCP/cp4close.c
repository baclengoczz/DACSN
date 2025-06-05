#include <p18cxxx.h>
#include <capture.h>

/********************************************************************
*    Function Name:    CloseCapture4                                *
*    Return Value:     void                                         *
*    Parameters:       void                                         *
*    Description:      This routine disables the capture interrupt. *
********************************************************************/
void CloseCapture4(void)
{
   PIE3bits.CCP4IE = 0; // Disable the interrupt
   CCP4CON=0x00;        // Reset the CCP module
}
