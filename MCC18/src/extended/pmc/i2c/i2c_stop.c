/* $Id: i2c_stop.c,v 1.1 2004/10/06 23:15:49 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    StopI2C                                     *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus stop condition.                *
********************************************************************/
#undef StopI2C
void StopI2C( void )
{
  SSPCON2bits.PEN = 1;            // initiate bus stop condition
}
