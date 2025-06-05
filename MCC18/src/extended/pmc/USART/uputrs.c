/* $Id: uputrs.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <usart.h>

/**********************************************************************
*    Function Name:  putrsUSART                                       *
*    Return Value:   void                                             *
*    Parameters:     data: pointer to string of data                  *
*    Description:    This routine transmits a string of characters    *
*                    in ROM to the USART including the null.          *
**********************************************************************/
void putrsUSART(const rom char *data)
{
  do
  {  // Transmit a byte
    while(BusyUSART());
    putcUSART(*data);
  } while( *data++ );
}
