#include <p18cxxx.h>
#include <usart.h>


/*********************************************************************
*    Function Name:  Busy1USART                                      *
*    Return Value:   char: transmit successful status                *
*    Parameters:     none                                            *
*    Description:    This routine checks to see if a byte can be     *
*                    written to the USART by checking the TRMT bit   *
*********************************************************************/
#undef Busy1USART
char Busy1USART(void)
{
  if(!TXSTA1bits.TRMT)  // Is the transmit shift register empty
    return 1;          // No, return FALSE

  return 0;            // Return TRUE
}
