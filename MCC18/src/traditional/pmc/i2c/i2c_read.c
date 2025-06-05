/* $Id: i2c_read.c,v 1.1 2004/10/06 23:16:42 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    ReadI2C                                     *
*     Return Value:     contents of SSPBUF register                 *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C bus.              *
********************************************************************/
unsigned char ReadI2C( void )
{
  SSPCON2bits.RCEN = 1;           // enable master for 1 byte reception
  while ( !SSPSTATbits.BF );      // wait until byte received  
  return ( SSPBUF );              // return with read byte 
}
