/* $Id: cp2open.c,v 1.2 2004/02/10 21:04:45 sealep Exp $ */
#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*    Function Name:  OpenCapture2                                   *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure capture   *
*    Description:    This routine configures the capture for        *
*                    interrupt, edge, and period or capture         *
*                    function.                                      *
*    Notes:          The bit definitions for config can be found    *
*                    in the capture.h file.                         *
********************************************************************/
void OpenCapture2(unsigned char config)
{
  CCP2CON = config&0x0F; //  Configure capture
 
  if(config&0x80)
  {
    PIR2bits.CCP2IF = 0;  // Clear the interrupt flag
    PIE2bits.CCP2IE = 1;  // Enable the interrupt
  }

  CapStatus.Cap2OVF = 0;  // Clear the capture overflow status flag
}
