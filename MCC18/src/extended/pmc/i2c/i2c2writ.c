/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    WriteI2C2                                   *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C2 bus.              *
*     Description:      This routine writes a single byte to the    * 
*                       I2C2 bus.                                   *
********************************************************************/
unsigned char WriteI2C2( unsigned char data_out )
{
  SSP2BUF = data_out;           // write single byte to SSP2BUF
  if ( SSP2CON1bits.WCOL )      // test if write collision occurred
    return ( -1 );              // if WCOL bit is set return negative #
  else
  {
    while( SSP2STATbits.BF );   // wait until write cycle is complete         
    return ( 0 );               // if WCOL bit is not set return non-negative #
  }
}
