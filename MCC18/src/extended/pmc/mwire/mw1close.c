/* $Id:  */
#include <p18cxxx.h>
#include <mwire.h>

/********************************************************************
*     Function Name:    CloseMwire1                                 *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP1 module. Pin *
*                       I/O returns under the control of the port   *
*                       registers.                                  * 
********************************************************************/
#undef CloseMwire1
void CloseMwire1( void )
{
  SSP1CON1 &= 0xDF;     // disable synchronous serial port
}
