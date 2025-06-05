/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    getsSPI2                                    *
*     Return Value:     void                                        *
*     Parameters:       address of read string storage location and *
*                       length of string bytes to read              *
*     Description:      This routine reads a string from the SPI2   *
*                       bus.  The number of bytes to read is deter- *
*                       mined by parameter 'length'.                *
********************************************************************/
void getsSPI2( unsigned char *rdptr, unsigned char length )
{
  while ( length )                // stay in loop until length = 0
  {
    *rdptr++ = getcSPI2();        // read a single byte
    length--;                     // reduce string length count by 1
  }
}
