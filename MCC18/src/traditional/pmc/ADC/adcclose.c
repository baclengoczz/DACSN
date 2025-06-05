/* $Id: adcclose.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/************************************************************************
*    Function Name:  CloseADC                                           *
*    Return Value:   void                                               *
*    Parameters:     void                                               *
*    Description:    This routine turns the A/D off and disables        *
*                    the A/D interrupt                                  *
************************************************************************/
void CloseADC(void)
{
  ADCON0bits.ADON = 0;
  PIE1bits.ADIE = 0;
}
