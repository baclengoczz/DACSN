#include <p18cxxx.h>
#include <usart.h>

// USART2 Status Structure
extern union USART2 USART2_Status;

/********************************************************************
*    Function Name:  Open2USART                                      *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure USART2     *
*                    spbrg: baudrate value for register SPBRG2      *
*    Description:    This routine first resets the USART2 regs       *
*                    to the POR state. It then configures the       *
*                    USART2 for interrupts, synch/async, 8/9-bit,    *
*                    sync slave/master and single/cont. rx.         *
*    Notes:          The bit definitions for config can be found    *
*                    in the usart.h file.                           *
********************************************************************/
void Open2USART( unsigned char config, char spbrg)
{
  TXSTA2 = 0;           // Reset USART registers to POR state
  RCSTA2 = 0;
 
  if(config&0x01)      // Sync or async operation
    TXSTA2bits.SYNC = 1;

  if(config&0x02)      // 8- or 9-bit mode
  {
    TXSTA2bits.TX9 = 1;
    RCSTA2bits.RX9 = 1;
  }

  if(config&0x04)      // Master or Slave (sync only)
    TXSTA2bits.CSRC = 1;

  if(config&0x08)      // Continuous or single reception
    RCSTA2bits.CREN = 1;
  else
    RCSTA2bits.SREN = 1;

  if(config&0x10)      // Baud rate select (asychronous mode only)
    TXSTA2bits.BRGH = 1;
  else
    TXSTA2bits.BRGH = 0;

  if(config&0x40)      // Interrupt on receipt
    PIE3bits.RC2IE = 1;
  else
    PIE3bits.RC2IE = 0;

  if(config&0x80)      // Interrupt on transmission
    PIE3bits.TX2IE = 1;
  else
    PIE3bits.TX2IE = 0;

  SPBRG2 = spbrg;       // Write baudrate to SPBRG2
  TXSTA2bits.TXEN = 1;  // Enable transmitter
  RCSTA2bits.SPEN = 1;  // Enable receiver
}
