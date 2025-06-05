/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    IdleI2C2                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Test and wait until I2C2 module is idle.    *
********************************************************************/
#undef IdleI2C2
void IdleI2C2( void )
{
  while ( ( SSP2CON2 & 0x1F ) | ( SSP2STATbits.R_W ) )
     continue;
}
