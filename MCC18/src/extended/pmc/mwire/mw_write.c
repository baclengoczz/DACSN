/* $Id: mw_write.c,v 1.1 2004/10/06 22:49:28 curtiss Exp $ */
#include <p18cxxx.h>
#include <mwire.h>

#if defined(__18F2331) || defined(__18F2431) || \
    defined(__18F4331) || defined(__18F4431)
#define SSPCON1 SSPCON
#define SSPCON1bits SSPCONbits
#endif

/********************************************************************
*     Function Name:    WriteMwire                                  *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for EEPROM device.         *
*     Description:      This routine writes a single byte to the    * 
*                       Microwire device.                           *
********************************************************************/
unsigned char WriteMwire( unsigned char data_out )
{
  SSPCON1bits.CKP = 1;         // ensure clock idle state is 1 for transmission
  SSPBUF = data_out;           // write single byte to the Microwire device
  if ( SSPCON1bits.WCOL )      // test if write collision occurred
   return ( -1 );              // if WCOL bit is set return negative #
  else
  {
   while ( !SSPSTATbits.BF );  // wait until bus cycle complete
   return ( 0 );               // if WCOL bit is not set return non negative #
  }
}
