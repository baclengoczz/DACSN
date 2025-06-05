/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/*********************************************************************
*     Function Name:    putsSPI1                                     *
*     Return Value:     void                                         *
*     Parameters:       address of write string storage location     *
*     Description:      This routine writes a string to the SPI1 bus.*  
*********************************************************************/
void putsSPI1( unsigned char *wrptr )
{
  while ( *wrptr )                 // test for string null character
  {
     SSP1BUF = *wrptr++;           // initiate SPI1 bus cycle
     while( !SSP1STATbits.BF );    // wait until 'BF' bit is set
  }
}
