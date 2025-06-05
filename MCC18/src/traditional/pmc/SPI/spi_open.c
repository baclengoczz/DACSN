/* $Id: spi_open.c,v 1.2 2005/04/11 21:22:35 nairnj Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*   Function Name:  OpenSPI                                         *
*   Return Value:   void                                            *
*   Parameters:     SSP peripheral setup bytes                      *
*   Description:    This function sets up the SSP module on a       * 
*                   PIC18Cxxx device for use with a Microchip SPI   *
*                   EEPROM device or SPI bus device.                *
********************************************************************/
void OpenSPI( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase)
{
  SSPSTAT &= 0x3F;                // power on state 
  SSPCON1 = 0x00;                 // power on state
  SSPCON1 |= sync_mode;           // select serial mode 
  SSPSTAT |= smp_phase;           // select data input sample phase

  switch( bus_mode )
  {
    case 0:                       // SPI bus mode 0,0
      SSPSTATbits.CKE = 1;        // data transmitted on rising edge
      break;    
    case 2:                       // SPI bus mode 1,0
      SSPSTATbits.CKE = 1;        // data transmitted on falling edge
      SSPCON1bits.CKP = 1;        // clock idle state high
      break;
    case 3:                       // SPI bus mode 1,1
      SSPCON1bits.CKP = 1;        // clock idle state high
      break;
    default:                      // default SPI bus mode 0,1
      break;
  }

  switch( sync_mode )
  {
    case 4:                       // slave mode w /SS enable
#if defined(__18F6310)  || defined(__18F6390)  || \
    defined(__18F6410)  || defined(__18F6490)  || \
    defined(__18F8310)  || defined(__18F8390)  || \
    defined(__18F8410)  || defined(__18F8490)  || \
    defined(__18F6527)  || defined(__18F6622)  || \
    defined(__18F6627)  || defined(__18F6722)  || \
    defined(__18F8527)  || defined(__18F8622)  || \
    defined(__18F8627)  || defined(__18F8722)  || \
    defined(__18F65J10) || defined(__18F65J15) || \
    defined(__18F66J10) || defined(__18F66J15) || \
    defined(__18F67J10) || defined(__18F85J10) || \
    defined(__18F85J15) || defined(__18F86J10) || \
    defined(__18F86J15) || defined(__18F87J10)
      TRISFbits.TRISF7 = 1;       // define /SS pin as input
#else
      TRISAbits.TRISA5 = 1;       // define /SS pin as input
#endif
    case 5:                       // slave mode w/o /SS enable
#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
      TRISBbits.TRISB1 = 1;       // define clock pin as input
#else
      TRISCbits.TRISC3 = 1;       // define clock pin as input
#endif
      SSPSTATbits.SMP = 0;        // must be cleared in slave SPI mode
      break;
    default:                      // master mode, define clock pin as output
#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
      TRISBbits.TRISB1 = 0;       // define clock pin as output
#else
      TRISCbits.TRISC3 = 0;       // define clock pin as output
#endif
      break;
  }

#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
  TRISC &= 0x7F;                  // define SDO as output (master or slave)
  TRISB |= 0x01;                  // define SDI as input (master or slave)
#else
  TRISC &= 0xDF;                  // define SDO as output (master or slave)
  TRISC |= 0x10;                  // define SDI as input (master or slave)
#endif
  SSPCON1 |= SSPENB;              // enable synchronous serial port 
}
