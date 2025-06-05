/* $Id: adcsetch.c,v 1.2 2004/02/10 21:01:55 sealep Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/********************************************************************
*    Function Name:  SetChanADC                                     *
*    Return Value:   void                                           *
*    Parameters:     channel: channel selection                     *
*    Description:    This routine sets the A/D to the specified     *
*                    channel                                        *
*    Notes:          The bit definitions for channel can be found   *
*                    in the adc.h file.                             *
********************************************************************/
void SetChanADC(unsigned char channel)
{
#ifdef ADC_INTERFACE_OLD
  ADCON0 = (channel & 0b00111000) |
           (ADCON0  & 0b11000111);
#else
  ADCON0 = ((channel >> 1) & 0b00111100) |
           (ADCON0  & 0b11000011);
#endif
}
