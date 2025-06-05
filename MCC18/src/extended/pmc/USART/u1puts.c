#include <p18cxxx.h>
#include <usart.h>

/**********************************************************************
*    Function Name:  puts1USART                                        *
*    Return Value:   void                                             *
*    Parameters:     data: pointer to string of data                  *
*    Description:    This routine transmits a string of characters    *
*                    to the USART1 including the null.                 *
**********************************************************************/
void puts1USART( char *data)
{
  do
  {  // Transmit a byte
    while(Busy1USART());
    putc1USART(*data);
  } while( *data++ );
}
