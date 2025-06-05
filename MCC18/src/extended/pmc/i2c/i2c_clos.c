#include <p18cxxx.h>
#include <i2c.h>

/********************************************************************
*     Function Name:    CloseI2C                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP module. Pin  *
*                       I/O returns under the control of the port   *
*                       registers.                                  *
********************************************************************/
#undef CloseI2C
void CloseI2C( void )
{
  SSPCON1 &= 0xDF;                // disable synchronous serial port
}
