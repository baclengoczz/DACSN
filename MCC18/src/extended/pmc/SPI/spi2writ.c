/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    WriteSPI2                                   *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for SPI2 bus.              *
*     Description:      This routine writes a single byte to the    * 
*                       SPI2 bus.                                   *
********************************************************************/
unsigned char WriteSPI2( unsigned char data_out )
{
  SSP2BUF = data_out;          // write byte to SSP2BUF register
  if ( SSP2CON1 & 0x80 )       // test if write collision occurred
   return ( -1 );              // if WCOL bit is set return negative #
  else
  {
    while( !SSP2STATbits.BF ); // wait until bus cycle complete 
  }
  return ( 0 );                // if WCOL bit is not set return non-negative#
}
