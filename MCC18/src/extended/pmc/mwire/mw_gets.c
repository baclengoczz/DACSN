/* $Id: mw_gets.c,v 1.1 2004/10/06 22:49:28 curtiss Exp $ */
#include <p18cxxx.h>
#include <mwire.h>

#if defined(__18F2331) || defined(__18F2431) || \
    defined(__18F4331) || defined(__18F4431)
#define SSPCON1 SSPCON
#define SSPCON1bits SSPCONbits
#endif

/********************************************************************
*     Function Name:    getsMwire                                   *
*     Return Value:     void                                        *
*     Parameters:       address of read string storage location and *
*                       length of string bytes to read              *
*     Description:      This routine reads a string from the        *
*                       Microwire device. User must first issue     *
*                       start bit, opcode and address before reading*
*                       a string. Function WriteMWire() or          *
*                       putcMWire() can be called 2 times to        *
*                       accomplish the byte write sequence.         *
********************************************************************/
void getsMwire( unsigned char *rdptr, unsigned char length )
{
  SSPCON1bits.CKP = 0;            // ensure clock idle state is 0 for read
  while ( length )                // stay in loop until length = 0
  {
    SSPBUF = 0x00;                // initiate bus cycle
    while( !SSPSTATbits.BF );     // wait until byte has been received
    *rdptr++ = SSPBUF;            // save byte
    length--;                     // reduce string byte count by 1
  }  
  SSPCON1bits.CKP = 1;            // ensure clock idle state is 1 for transmission
}
