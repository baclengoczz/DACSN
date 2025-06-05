#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    CloseI2C2                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP2 module. Pin *
*                       I/O returns under the control of the port   *
*                       registers.                                  *
********************************************************************/
#undef CloseI2C2
void CloseI2C2( void )
{
  SSP2CON1 &= 0xDF;                // disable synchronous serial port
}
