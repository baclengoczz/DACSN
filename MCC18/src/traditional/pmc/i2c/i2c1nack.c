/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    NotAckI2C1                                  *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Initiate NOT ACK bus condition.             *
********************************************************************/
#undef NotAckI2C1
void NotAckI2C1( void )
{
  SSP1CON2bits.ACKDT = 1;          // set acknowledge bit for not ACK
  SSP1CON2bits.ACKEN = 1;          // initiate bus acknowledge sequence
}
