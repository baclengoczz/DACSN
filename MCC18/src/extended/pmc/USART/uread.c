/* $Id: uread.c,v 1.2 2005/06/28 19:04:36 nairnj Exp $ */
#include <p18cxxx.h>
#include <usart.h>

extern union USART USART_Status;

/********************************************************************
*    Function Name:  ReadUSART                                      *
*    Return Value:   char: received data                            *
*    Parameters:     void                                           *
*    Description:    This routine reads the data from the USART     *
*                    and records the status flags for that byte     *
*                    in USART_Status (Framing and Overrun).         *
********************************************************************/
char ReadUSART(void)
{
  char data;   // Holds received data

  USART_Status.val &= 0xf2;          // Clear previous status flags

  if(RCSTAbits.RX9)                  // If 9-bit mode
  {
    USART_Status.RX_NINE = 0;        // Clear the receive bit 9 for USART
    if(RCSTAbits.RX9D)               // according to the RX9D bit
      USART_Status.RX_NINE = 1;
  }

  if(RCSTAbits.FERR)                 // If a framing error occured
    USART_Status.FRAME_ERROR = 1;    // Set the status bit

  if(RCSTAbits.OERR)                 // If an overrun error occured
    USART_Status.OVERRUN_ERROR = 1;  // Set the status bit

  data = RCREG;                      // Read data

  return (data);                     // Return the received data
}


