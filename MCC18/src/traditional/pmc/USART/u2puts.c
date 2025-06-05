#include <p18cxxx.h>
#include <usart.h>

/**********************************************************************
*    Function Name:  puts2USART                                        *
*    Return Value:   void                                             *
*    Parameters:     data: pointer to string of data                  *
*    Description:    This routine transmits a string of characters    *
*                    to the USART2 including the null.                 *
**********************************************************************/
void puts2USART( char *data)
{
  do
  {  // Transmit a byte
    while(Busy2USART());
    putc2USART(*data);
  } while( *data++ );
}
