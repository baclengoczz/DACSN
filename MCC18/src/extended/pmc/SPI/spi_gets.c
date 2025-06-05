/* $Id: spi_gets.c,v 1.1 2004/10/06 23:00:07 curtiss Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    getsSPI                                     *
*     Return Value:     void                                        *
*     Parameters:       address of read string storage location and *
*                       length of string bytes to read              *
*     Description:      This routine reads a string from the SPI    *
*                       bus.  The number of bytes to read is deter- *
*                       mined by parameter 'length'.                *
********************************************************************/
void getsSPI( unsigned char *rdptr, unsigned char length )
{
  while ( length )                // stay in loop until length = 0
  {
    *rdptr++ = getcSPI();         // read a single byte
    length--;                     // reduce string length count by 1
  }
}
