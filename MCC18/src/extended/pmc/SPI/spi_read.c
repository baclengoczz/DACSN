/* $Id: spi_read.c,v 1.1 2004/10/06 23:00:07 curtiss Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    ReadSPI                                     *
*     Return Value:     contents of SSPBUF register                 *
*     Parameters:       void                                        *
*     Description:      Read single byte from SPI bus.              *
********************************************************************/
unsigned char ReadSPI( void )
{
  SSPBUF = 0x00;                  // initiate bus cycle
  while ( !SSPSTATbits.BF );      // wait until cycle complete
  return ( SSPBUF );              // return with byte read 
}
