/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    RestartI2C1                                 *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C1 bus restart condition.            *
********************************************************************/
#undef RestartI2C1
void RestartI2C1( void )
{
  SSP1CON2bits.RSEN = 1;           // initiate bus restart condition
}
