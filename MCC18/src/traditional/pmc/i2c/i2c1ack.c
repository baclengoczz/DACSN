/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    AckI2C1                                      *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Initiate ACK bus condition.                 *
********************************************************************/
#undef AckI2C1
void AckI2C1( void )
{
  SSP1CON2bits.ACKDT = 0;           // set acknowledge bit state for ACK
  SSP1CON2bits.ACKEN = 1;           // initiate bus acknowledge sequence
}
