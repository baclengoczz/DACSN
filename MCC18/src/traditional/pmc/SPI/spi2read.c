/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    ReadSPI2                                    *
*     Return Value:     contents of SSP2BUF register                *
*     Parameters:       void                                        *
*     Description:      Read single byte from SPI2 bus.             *
********************************************************************/
unsigned char ReadSPI2( void )
{
  SSP2BUF = 0x00;                  // initiate bus cycle
  while ( !SSP2STATbits.BF );      // wait until cycle complete
  return ( SSP2BUF );              // return with byte read 
}
