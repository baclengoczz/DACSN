/* $Id: mw_drdy.c,v 1.1 2004/10/06 22:49:28 curtiss Exp $ */
#include <p18cxxx.h>
#include <mwire.h>

/********************************************************************
*     Function Name:    DataRdyMwire                                *
*     Return Value:     status byte to indicate ready/busy          *
*     Parameters:       void                                        *
*     Description:      Determine if Microwire device is ready,     *
*                       write cycle complete. If used after a       *
*                       write/erase cycle is complete a pull-up     *
*                       resistor on the DI line is required. If the *
*                       routine is called after a cycle has been    *
*                       initiated while CS is high then no pull-up  *
*                       resistor is required.                       *
********************************************************************/
#undef DataRdyMwire
unsigned char DataRdyMwire( void )
{
#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
  if ( PORTBbits.RB0 )            // test if DI line is high    
#else
  if ( PORTCbits.RC4 )            // test if DI line is high    
#endif
    return ( +1 );                // return ready = true
  else
    return ( 0 );                 // else ready = false
}
