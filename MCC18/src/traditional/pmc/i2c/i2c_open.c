/* $Id: i2c_open.c,v 1.1 2004/10/06 23:16:42 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*   Function Name:  OpenI2C                                         *
*   Return Value:   void                                            *
*   Parameters:     SSP peripheral setup bytes                      *
*   Description:    This function sets up the SSP module on a       * 
*                   PIC18CXXX device for use with a Microchip I2C   *
*                   EEPROM device or I2C bus device.                *
********************************************************************/
void OpenI2C( unsigned char sync_mode, unsigned char slew )
{
  SSPSTAT &= 0x3F;                // power on state 
  SSPCON1 = 0x00;                 // power on state
  SSPCON2 = 0x00;                 // power on state
  SSPCON1 |= sync_mode;           // select serial mode 
  SSPSTAT |= slew;                // slew rate on/off 

#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
  DDRBbits.RB1 = 1;               // Set SCL (PORTB,1) pin to input
  DDRBbits.RB0 = 1;               // Set SDA (PORTB,0) pin to input
#else
  DDRCbits.RC3 = 1;               // Set SCL (PORTC,3) pin to input
  DDRCbits.RC4 = 1;               // Set SDA (PORTC,4) pin to input
#endif

  SSPCON1 |= SSPENB;              // enable synchronous serial port 
}
