/* $Id: adcconv.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/********************************************************************
*    Function Name:  ConvertADC                                     *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine starts an A/D conversion by       *
*                    setting the GO bit.                            *
********************************************************************/
void ConvertADC(void)
{
  ADCON0bits.GO = 1;  // Set the GO bit to start a conversion
}
