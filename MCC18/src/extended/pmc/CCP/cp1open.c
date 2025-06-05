/* $Id: cp1open.c,v 1.2 2004/02/10 21:03:49 sealep Exp $ */
#include <p18cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*    Function Name:  OpenCapture1                                   *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure capture   *
*    Description:    This routine configures the capture for        *
*                    interrupt, edge, and period or capture         *
*                    function.                                      *
*    Notes:          The bit definitions for config can be found    *
*                    in the capture.h file.                         *
********************************************************************/
void OpenCapture1(unsigned char config)
{
  CCP1CON = config&0x0F;  // Configure capture
 
  if(config&0x80)
  {
    PIR1bits.CCP1IF = 0;   // Clear the interrupt flag
    PIE1bits.CCP1IE = 1;   // Enable the interrupt

  }
  CapStatus.Cap1OVF = 0;   // Clear the capture overflow status flag
}
