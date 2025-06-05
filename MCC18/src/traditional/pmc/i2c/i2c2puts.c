/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/*********************************************************************
*     Function Name:    putsI2C2                                     *
*     Return Value:     error condition status                       *
*     Parameters:       address of write string storage location     *
*     Description:      This routine writes a string to the I2C2 bus,*
*                       until a null character is reached. If Master *
*                       function putcI2C is called. When trans-      *
*                       mission is complete then test for ack-       *
*                       nowledge bit. If Slave transmitter wait for  *
*                       null character or not ACK received from bus  *
*                       device.                                      *
*********************************************************************/
signed char putsI2C2( unsigned char *wrptr )
{
  while ( *wrptr )                 // transmit data until null character 
  {
    if ( SSP2CON1bits.SSPM3 )      // if Master transmitter then execute the following
    {
      if ( putcI2C2( *wrptr ) )    // write 1 byte
      {
        return ( -3 );             // return with write collision error
      }
      IdleI2C2();                  // test for idle condition
      if ( SSP2CON2bits.ACKSTAT )  // test received ack bit state
      {
        return ( -2 );             // bus device responded with  NOT ACK
      }                            // terminate putsI2C2() function
    }

    else                           // else Slave transmitter
    {
      PIR3bits.SSP2IF = 0;         // reset SSP2IF bit
      SSP2BUF = *wrptr;            // load SSP2BUF with new data
      SSP2CON1bits.CKP = 1;        // release clock line 
      while ( !PIR3bits.SSP2IF );  // wait until ninth clock pulse received

      if ( ( !SSP2STATbits.R_W ) && ( !SSP2STATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
      {
        return ( -2 );             // terminate PutsI2C2() function
      }
    }

  wrptr ++;                        // increment pointer

  }                                // continue data writes until null character

  return ( 0 );
}
