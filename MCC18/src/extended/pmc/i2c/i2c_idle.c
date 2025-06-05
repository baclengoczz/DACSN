/* $Id: i2c_idle.c,v 1.1 2004/10/06 23:15:49 curtiss Exp $ */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    IdleI2C                                     *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Test and wait until I2C module is idle.     *
********************************************************************/
#undef IdleI2C
void IdleI2C( void )
{
  while ( ( SSPCON2 & 0x1F ) | ( SSPSTATbits.R_W ) )
     continue;
}
