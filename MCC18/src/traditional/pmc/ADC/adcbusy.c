/* $Id: adcbusy.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/****************************************************************
*    Function Name:  BusyADC                                    *
*    Return Value:   char: 1 - A/D busy, 0 - Done converting    *
*    Parameters:     void                                       *
*    Description:    This routine checks the GO bit to see if   *
*                    the A/D conversion is done.                *
****************************************************************/
char BusyADC(void)
{
  /* If the GO bit is set then A/D is busy converting
   * Otherwise A/D is done converting
   */
  return(ADCON0bits.GO);

}
