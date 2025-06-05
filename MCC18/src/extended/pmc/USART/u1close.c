#include <p18cxxx.h>
#include <usart.h>

/*********************************************************************
*    Function Name:  Close1USART                                      *
*    Return Value:   void                                            *
*    Parameters:     void                                            *
*    Description:    This routine disables the transmitter and       *
*                    receiver and disables the interrupts for both   *
*    Note:           There is a macro version of this function       *
*                    available in usart.h                            *
*********************************************************************/
#undef Close1USART
void Close1USART(void)
{
  RCSTA1 &= 0b01001111;  // Disable the receiver
  TXSTA1bits.TXEN = 0;   // and transmitter

  PIE1 &= 0b11001111;   // Disable both interrupts
}
