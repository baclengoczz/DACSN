/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*   Function Name:  OpenI2C2                                        *
*   Return Value:   void                                            *
*   Parameters:     SSP2 peripheral setup bytes                     *
*   Description:    This function sets up the SSP2 module on a      * 
*                   PIC18CXXX device for use with a Microchip I2C   *
*                   EEPROM device or I2C bus device.                *
********************************************************************/
void OpenI2C2( unsigned char sync_mode, unsigned char slew )
{
  SSP2STAT &= 0x3F;                // power on state 
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON2 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode 
  SSP2STAT |= slew;                // slew rate on/off 

#if defined(__18F45J10) || defined(__18F44J10)
  DDRDbits.RD0 = 1;               // Set SCL2 (PORTD,0) pin to input
  DDRDbits.RD1 = 1;               // Set SDA2 (PORTD,1) pin to input
#else
  DDRDbits.RD6 = 1;               // Set SCL2 (PORTD,6) pin to input
  DDRDbits.RD5 = 1;               // Set SDA2 (PORTD,5) pin to input
#endif

  SSP2CON1 |= SSPENB;              // enable synchronous serial port 
}
