/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    ReadI2C1                                    *
*     Return Value:     contents of SSP1BUF register                *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C1 bus.             *
********************************************************************/
unsigned char ReadI2C1( void )
{
  SSP1CON2bits.RCEN = 1;           // enable master for 1 byte reception
  while ( !SSP1STATbits.BF );      // wait until byte received  
  return ( SSP1BUF );              // return with read byte 
}
