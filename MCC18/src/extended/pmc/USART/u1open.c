#include <p18cxxx.h>
#include <usart.h>

#if defined(__18F6525) || defined(__18F6621) || \
    defined(__18F8525) || defined(__18F8621)
#define RC1IE RCIE
#define TX1IE TXIE
#endif

// USART1 Status Structure
extern union USART1 USART1_Status;

/********************************************************************
*    Function Name:  Open1USART                                      *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure USART1     *
*                    spbrg: baudrate value for register SPBRG1      *
*    Description:    This routine first resets the USART1 regs       *
*                    to the POR state. It then configures the       *
*                    USART1 for interrupts, synch/async, 8/9-bit,    *
*                    sync slave/master and single/cont. rx.         *
*    Notes:          The bit definitions for config can be found    *
*                    in the usart.h file.                           *
********************************************************************/
void Open1USART( unsigned char config, char spbrg)
{
  TXSTA1 = 0;           // Reset USART registers to POR state
  RCSTA1 = 0;
 
  if(config&0x01)      // Sync or async operation
    TXSTA1bits.SYNC = 1;

  if(config&0x02)      // 8- or 9-bit mode
  {
    TXSTA1bits.TX9 = 1;
    RCSTA1bits.RX9 = 1;
  }

  if(config&0x04)      // Master or Slave (sync only)
    TXSTA1bits.CSRC = 1;

  if(config&0x08)      // Continuous or single reception
    RCSTA1bits.CREN = 1;
  else
    RCSTA1bits.SREN = 1;

  if(config&0x10)      // Baud rate select (asychronous mode only)
    TXSTA1bits.BRGH = 1;
  else
    TXSTA1bits.BRGH = 0;

  if(config&0x40)      // Interrupt on receipt
    PIE1bits.RC1IE = 1;
  else
    PIE1bits.RC1IE = 0;

  if(config&0x80)      // Interrupt on transmission
    PIE1bits.TX1IE = 1;
  else
    PIE1bits.TX1IE = 0;

  SPBRG1 = spbrg;       // Write baudrate to SPBRG1
  TXSTA1bits.TXEN = 1;  // Enable transmitter
  RCSTA1bits.SPEN = 1;  // Enable receiver
}
