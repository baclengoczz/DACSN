/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    StartI2C1                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C1 bus start condition.              *
********************************************************************/
#undef StartI2C1
void StartI2C1( void )
{
  SSP1CON2bits.SEN = 1;            // initiate bus start condition
}
