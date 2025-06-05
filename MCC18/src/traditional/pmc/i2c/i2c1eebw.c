/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    EEByteWrite1                                *
*     Return Value:     error condition status                      *
*     Parameters:       EE memory device control, address and data  *
*                       bytes.                                      *
*     Description:      Write single data byte to I2C1 EE memory    *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which only uses 1 byte of *
*                       address data as in the 24LC01B/02B/04B/08B. *
*                                                                   *  
********************************************************************/
unsigned char EEByteWrite1( unsigned char control, unsigned char address, unsigned char data )
{
  IdleI2C1();                      // ensure module is idle
  StartI2C1();                     // initiate START condition
  while ( SSP1CON2bits.SEN );      // wait until start condition is over 
  if ( PIR2bits.BCL1IF )           // test for bus collision
  {
    return ( -1 );                 // return with Bus Collision error 
  }
  else                             // start condition successful
  {
    if ( WriteI2C1( control ) )    // write byte - R/W bit should be 0
    {
      return ( -3 );               // set error for write collision
    }

    IdleI2C1();                    // ensure module is idle
    if ( !SSP1CON2bits.ACKSTAT )   // test for ACK condition received
    { 
      if ( WriteI2C1( address ) )  // write word address for EEPROM
      {
        return ( -3 );             // set error for write collision
      }

      IdleI2C1();                  // ensure module is idle
      if ( !SSP1CON2bits.ACKSTAT ) // test for ACK condition received
      { 
        if ( WriteI2C1( data ) )   // data byte for EEPROM
        {
          return ( -3 );           // set error for write collision
        }
      }
      else
      {
        return ( -2 );             // return with Not Ack error condition   
      }
    }
    else
    {
      return ( -2 );               // return with Not Ack error condition   
    }
  }

  IdleI2C1();                      // ensure module is idle  
  StopI2C1();                      // send STOP condition
  while ( SSP1CON2bits.PEN );      // wait until stop condition is over 
  if ( PIR2bits.BCL1IF )           // test for bus collision
  {
    return ( -1 );                // return with Bus Collision error 
  }
  return ( 0 );                   // return with no error
}
