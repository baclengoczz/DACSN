/* $Id: i2c_strt.c,v 1.1 2004/10/06 23:16:42 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    StartI2C                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus start condition.               *
********************************************************************/
#undef StartI2C
void StartI2C( void )
{
  SSPCON2bits.SEN = 1;            // initiate bus start condition
}
