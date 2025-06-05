/* $Id: spi_dtrd.c,v 1.1 2004/10/06 23:00:51 curtiss Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    DataRdySPI                                  *
*     Return Value:     status byte to indicate if BF = 1           *
*     Parameters:       void                                        *
*     Description:      Determine if there is a byte to be read     *
*                       from the SSPBUF register.                   *
********************************************************************/
#undef DataRdySPI
unsigned char DataRdySPI( void )
{
  if ( SSPSTATbits.BF )
    return ( +1 );                // data in SSPBUF register
  else
    return ( 0 );                 // no data in SSPBUF register
}
