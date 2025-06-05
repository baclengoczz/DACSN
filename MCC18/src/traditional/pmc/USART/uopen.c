/* $Id: uopen.c,v 1.5 2005/04/11 21:22:35 nairnj Exp $ */
#include <p18cxxx.h>
#include <usart.h>

// USART Status Structure
extern union USART USART_Status;

/********************************************************************
*    Function Name:  OpenUSART                                      *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure USART     *
*                    spbrg: baudrate value for register SPBRG1      *
*                    or for SPBRGH:SPBRG for 16-bit baud rate       *
*                    generation for applicable parts                *
*    Description:    This routine first resets the USART regs       *
*                    to the POR state. It then configures the       *
*                    USART for interrupts, synch/async, 8/9-bit,    *
*                    sync slave/master and single/cont. rx.         *
*    Notes:          The bit definitions for config can be found    *
*                    in the usart.h file.                           *
********************************************************************/
void OpenUSART( unsigned char config, unsigned spbrg)
{
  TXSTA = 0;           // Reset USART registers to POR state
  RCSTA = 0;
 
  if(config&0x01)      // Sync or async operation
    TXSTAbits.SYNC = 1;

  if(config&0x02)      // 8- or 9-bit mode
  {
    TXSTAbits.TX9 = 1;
    RCSTAbits.RX9 = 1;
  }

  if(config&0x04)      // Master or Slave (sync only)
    TXSTAbits.CSRC = 1;

  if(config&0x08)      // Continuous or single reception
    RCSTAbits.CREN = 1;
  else
    RCSTAbits.SREN = 1;

  if(config&0x10)      // Baud rate select (asychronous mode only)
    TXSTAbits.BRGH = 1;
  else
    TXSTAbits.BRGH = 0;

  PIR1bits.TXIF = 0;

  if(config&0x40)      // Interrupt on receipt
    PIE1bits.RCIE = 1;
  else
    PIE1bits.RCIE = 0;

  PIR1bits.RCIF = 0;

  if(config&0x80)      // Interrupt on transmission
    PIE1bits.TXIE = 1;
  else
    PIE1bits.TXIE = 0;

  SPBRG = spbrg;       // Write baudrate to SPBRG1
#if defined(__18F1220)  || defined(__18F1320)  || \
    defined(__18F2480)  || defined(__18F2580)  || \
    defined(__18F4480)  || defined(__18F4580)  || \
    defined(__18F2585)  || defined(__18F2680)  || \
    defined(__18F4585)  || defined(__18F4680)  || \
    defined(__18F2515)  || defined(__18F2525)  || \
    defined(__18F2610)  || defined(__18F2620)  || \
    defined(__18F4515)  || defined(__18F4525)  || \
    defined(__18F4610)  || defined(__18F4620)  || \
    defined(__18F2331)  || defined(__18F2410)  || \
    defined(__18F2420)  || defined(__18F2431)  || \
    defined(__18F2455)  || defined(__18F4455)  || \
    defined(__18F24J10) || defined(__18F2510)  || \
    defined(__18F2520)  || defined(__18F2550)  || \
    defined(__18F25J10) || defined(__18F45J10) || \
    defined(__18F4331)  || defined(__18F4410)  || \
    defined(__18F4420)  || defined(__18F4431)  || \
    defined(__18F44J10) || defined(__18F4510)  || \
    defined(__18F4520)  || defined(__18F4550)  || \
    defined(__18F6585)  || defined(__18F6680)  || \
    defined(__18F8585)  || defined(__18F8680)  
  SPBRGH = spbrg >> 8; // For 16-bit baud rate generation
#endif
  TXSTAbits.TXEN = 1;  // Enable transmitter
  RCSTAbits.SPEN = 1;  // Enable receiver
}
