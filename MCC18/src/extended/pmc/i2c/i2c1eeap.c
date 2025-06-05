/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/*********************************************************************
*     Function Name:    EEAckPolling1                                *
*     Return Value:     error condition status                       *
*     Parameters:       EE memory control byte                       *
*     Description:      Acknowledge polling of I2C1 EE memory        *
*                       device. This routine can be used for most    *
*                       I2C EE memory devices which uses acknowledge *
*                       polling.                                     *
*********************************************************************/
unsigned char EEAckPolling1( unsigned char control )
{
  IdleI2C1();                      // ensure module is idle 
  StartI2C1();                     // initiate START condition
  while ( SSP1CON2bits.SEN );      // wait until start condition is over 
  if ( PIR2bits.BCL1IF )           // test for bus collision
  {
    return ( -1 );                 // return with Bus Collision error 
  }

  else
  {
    if ( WriteI2C1( control ) )    // write byte - R/W bit should be 0
    {
      return ( -3 );               // set error for write collision
    }

    IdleI2C1();                    // ensure module is idle   
    if ( PIR2bits.BCL1IF )         // test for bus collision
    {
      return ( -1 );               // return with Bus Collision error 
    }

    while ( SSP1CON2bits.ACKSTAT ) // test for ACK condition received
    {
      RestartI2C1();               // initiate Restart condition
      while ( SSP1CON2bits.RSEN ); // wait until re-start condition is over 
      if ( PIR2bits.BCL1IF )       // test for bus collision
      {
        return ( -1 );             // return with Bus Collision error 
      }

      if ( WriteI2C1( control ) )  // write byte - R/W bit should be 0
      {
        return ( -3 );             // set error for write collision
      }
      IdleI2C1();                  // ensure module is idle
    }
  }

         
  StopI2C1();                      // send STOP condition
  while ( SSP1CON2bits.PEN );      // wait until stop condition is over         
  if ( PIR2bits.BCL1IF )           // test for bus collision
  {
    return ( -1 );                 // return with Bus Collision error 
  }
  return ( 0 );                    // return with no error     
}
