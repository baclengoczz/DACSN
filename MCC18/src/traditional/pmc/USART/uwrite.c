/* $Id: uwrite.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <usart.h>

extern union USART USART_Status;

/********************************************************************
*    Function Name:  WriteUSART                                     *
*    Return Value:   none                                           *
*    Parameters:     data: data to transmit                         *
*    Description:    This routine transmits a byte out the USART.   *
********************************************************************/
void WriteUSART(char data)
{
  if(TXSTAbits.TX9)  // 9-bit mode?
  {
    TXSTAbits.TX9D = 0;       // Set the TX9D bit according to the
    if(USART_Status.TX_NINE)  // USART Tx 9th bit in status reg
      TXSTAbits.TX9D = 1;
  }

  TXREG = data;      // Write the data byte to the USART
}
