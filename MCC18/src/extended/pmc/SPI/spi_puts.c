/* $Id: spi_puts.c,v 1.1 2004/10/06 23:00:07 curtiss Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    putsSPI                                     *
*     Return Value:     void                                        *
*     Parameters:       address of write string storage location    *
*     Description:      This routine writes a string to the SPI bus.*  
********************************************************************/
void putsSPI( unsigned char *wrptr )
{
  while ( *wrptr )                // test for string null character
  {
     SSPBUF = *wrptr++;           // initiate SPI bus cycle
     while( !SSPSTATbits.BF );    // wait until 'BF' bit is set
  }
}
