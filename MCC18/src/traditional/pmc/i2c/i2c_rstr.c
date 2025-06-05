/* $Id: i2c_rstr.c,v 1.1 2004/10/06 23:16:42 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    RestartI2C                                  *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus restart condition.             *
********************************************************************/
#undef RestartI2C
void RestartI2C( void )
{
  SSPCON2bits.RSEN = 1;           // initiate bus restart condition
}
