#include <p18cxxx.h>
#include <usart.h>

extern union USART2 USART2_Status;

/********************************************************************
*    Function Name:  Write2USART                                     *
*    Return Value:   none                                           *
*    Parameters:     data: data to transmit                         *
*    Description:    This routine transmits a byte out the USART2.   *
********************************************************************/
void Write2USART(char data)
{
  if(TXSTA2bits.TX9)  // 9-bit mode?
  {
    TXSTA2bits.TX9D = 0;       // Set the TX9D bit according to the
    if(USART2_Status.TX_NINE)  // USART2 Tx 9th bit in status reg
      TXSTA2bits.TX9D = 1;
  }

  TXREG2 = data;      // Write the data byte to the USART2
}
