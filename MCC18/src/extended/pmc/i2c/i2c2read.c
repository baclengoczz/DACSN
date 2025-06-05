/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    ReadI2C2                                    *
*     Return Value:     contents of SSP2BUF register                *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C2 bus.             *
********************************************************************/
unsigned char ReadI2C2( void )
{
  SSP2CON2bits.RCEN = 1;           // enable master for 1 byte reception
  while ( !SSP2STATbits.BF );      // wait until byte received  
  return ( SSP2BUF );              // return with read byte 
}
