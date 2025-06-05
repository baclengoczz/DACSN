/* $Id: i2c_writ.c,v 1.1 2004/10/06 23:15:49 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    WriteI2C                                    *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C bus.               *
*     Description:      This routine writes a single byte to the    * 
*                       I2C bus.                                    *
********************************************************************/
unsigned char WriteI2C( unsigned char data_out )
{
  SSPBUF = data_out;           // write single byte to SSPBUF
  if ( SSPCON1bits.WCOL )      // test if write collision occurred
   return ( -1 );              // if WCOL bit is set return negative #
  else
  {
    while( SSPSTATbits.BF );   // wait until write cycle is complete         
    return ( 0 );              // if WCOL bit is not set return non-negative #
  }
}
