/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*   Function Name:  OpenSPI2                                        *
*   Return Value:   void                                            *
*   Parameters:     SSP2 peripheral setup bytes                     *
*   Description:    This function sets up the SSP2 module on a      * 
*                   PIC18Cxxx device for use with a Microchip SPI   *
*                   EEPROM device or SPI bus device.                *
********************************************************************/
void OpenSPI2( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase)
{
  SSP2STAT &= 0x3F;               // power on state 
  SSP2CON1 = 0x00;                // power on state
  SSP2CON1 |= sync_mode;          // select serial mode 
  SSP2STAT |= smp_phase;          // select data input sample phase

  switch( bus_mode )
  {
    case 0:                       // SPI2 bus mode 0,0
      SSP2STATbits.CKE = 1;       // data transmitted on rising edge
      break;    
    case 2:                       // SPI2 bus mode 1,0
      SSP2STATbits.CKE = 1;       // data transmitted on falling edge
      SSP2CON1bits.CKP = 1;       // clock idle state high
      break;
    case 3:                       // SPI2 bus mode 1,1
      SSP2CON1bits.CKP = 1;       // clock idle state high
      break;
    default:                      // default SPI2 bus mode 0,1
      break;
  }

  switch( sync_mode )
  {
    case 4:                       // slave mode w /SS2 enable
#if defined(__18F44J10) || defined(__18F45J10)
      TRISDbits.TRISD3 = 1;       // define /SS2 pin as input
#else
      TRISDbits.TRISD7 = 1;       // define /SS2 pin as input
#endif
    case 5:                       // slave mode w/o /SS2 enable
#if defined(__18F44J10) || defined(__18F45J10)
      TRISDbits.TRISD0 = 1;       // define clock pin as input
#else
      TRISDbits.TRISD6 = 1;       // define clock pin as input
#endif
      SSP2STATbits.SMP = 0;       // must be cleared in slave SPI mode
      break;
    default:                      // master mode, define clock pin as output
#if defined(__18F44J10) || defined(__18F45J10)
      TRISDbits.TRISD0 = 0;       // define clock pin as output
#else
      TRISDbits.TRISD6 = 0;       // define clock pin as output
#endif
      break;
  }

#if defined(__18F44J10) || defined(__18F45J10)
  TRISD &= 0xFB;                  // define SDO2 as output (master or slave)
  TRISD |= 0x02;                  // define SDI2 as input (master or slave)
#else
  TRISD &= 0xEF;                  // define SDO2 as output (master or slave)
  TRISD |= 0x20;                  // define SDI2 as input (master or slave)
#endif
  SSP2CON1 |= SSPENB;             // enable synchronous serial port 
}
