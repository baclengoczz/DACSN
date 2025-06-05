/* $Id:  */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    CloseSPI2                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP2 module. Pin *
*                       I/O returns under the control of the port   *
*                       registers.                                  *
********************************************************************/
#undef CloseSPI2
void CloseSPI2( void )
{
  SSP2CON1 &= 0xDF;                // disable synchronous serial port
}
