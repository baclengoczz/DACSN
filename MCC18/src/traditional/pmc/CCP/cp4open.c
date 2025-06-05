#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*    Function Name:  OpenCapture4                                   *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure capture   *
*    Description:    This routine configures the capture for        *
*                    interrupt, edge, and period or capture         *
*                    function.                                      *
*    Notes:          The bit definitions for config can be found    *
*                    in the capture.h file.                         *
********************************************************************/
void OpenCapture4(unsigned char config)
{
  CCP4CON = config&0x0F; //  Configure capture
 
  if(config&0x80)
  {
    PIR3bits.CCP4IF = 0;  // Clear the interrupt flag
    PIE3bits.CCP4IE = 1;  // Enable the interrupt
  }

  CapStatus.Cap4OVF = 0;  // Clear the capture overflow status flag
}
