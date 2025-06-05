/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    ReadSPI1                                    *
*     Return Value:     contents of SSP1BUF register                *
*     Parameters:       void                                        *
*     Description:      Read single byte from SPI1 bus.             *
********************************************************************/
unsigned char ReadSPI1( void )
{
  SSP1BUF = 0x00;                  // initiate bus cycle
  while ( !SSP1STATbits.BF );      // wait until cycle complete
  return ( SSP1BUF );              // return with byte read 
}
