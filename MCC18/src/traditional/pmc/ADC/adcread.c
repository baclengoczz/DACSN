/* $Id: adcread.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/********************************************************************
*    Function Name:  ReadADC                                        *
*    Return Value:   int, A/D result                                *
*    Parameters:     void                                           *
*    Description:    This routine reads the ADRESL and ADRESH       *
*                    and returns these as a long.                   *
********************************************************************/
int ReadADC(void)
{
  union ADCResult i; // A union is used to read the
                     // A/D result due to issues with
                     // handling long variables

  i.br[0] = ADRESL;  // Read ADRESL into the lower byte
  i.br[1] = ADRESH;  // Read ADRESH into the high byte

  return (i.lr);     // Return the long variable
}
