/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    StartI2C2                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C2 bus start condition.              *
********************************************************************/
#undef StartI2C2
void StartI2C2( void )
{
  SSP2CON2bits.SEN = 1;            // initiate bus start condition
}
