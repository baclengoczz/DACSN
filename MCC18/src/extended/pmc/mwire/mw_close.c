/* $Id: mw_close.c,v 1.1 2004/10/06 22:49:28 curtiss Exp $ */
#include <p18cxxx.h>
#include <mwire.h>

#if defined(__18F2331) || defined(__18F2431) || \
    defined(__18F4331) || defined(__18F4431)
#define SSPCON1 SSPCON
#define SSPCON1bits SSPCONbits
#endif

/********************************************************************
*     Function Name:    CloseMwire                                  *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP module. Pin  *
*                       I/O returns under the control of the port   *
*                       registers.                                  * 
********************************************************************/
#undef CloseMwire
void CloseMwire( void )
{
  SSPCON1 &= 0xDF;     // disable synchronous serial port
}
