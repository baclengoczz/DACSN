/* $Id: i2c_puts.c,v 1.2 2005/04/11 21:22:35 nairnj Exp $ */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    putsI2C                                     *
*     Return Value:     error condition status                      *
*     Parameters:       address of write string storage location    *
*     Description:      This routine writes a string to the I2C bus,*
*                       until a null character is reached. If Master*
*                       function putcI2C is called. When trans-     *
*                       mission is complete then test for ack-      *
*                       nowledge bit. If Slave transmitter wait for *
*                       null character or not ACK received from bus *
*                       device.                                     *
********************************************************************/
signed char putsI2C( unsigned char *wrptr )
{
  while ( *wrptr )                // transmit data until null character 
  {
    if ( SSPCON1bits.SSPM3 )      // if Master transmitter then execute the following
    {
      if ( putcI2C ( *wrptr ) )   // write 1 byte
      {
        return ( -3 );            // return with write collision error
      }
      IdleI2C();                  // test for idle condition
      if ( SSPCON2bits.ACKSTAT )  // test received ack bit state
      {
        return ( -2 );            // bus device responded with  NOT ACK
      }                           // terminate putsI2C() function
    }

    else                          // else Slave transmitter
    {
      PIR1bits.SSPIF = 0;         // reset SSPIF bit
      SSPBUF = *wrptr;            // load SSPBUF with new data
      SSPCON1bits.CKP = 1;        // release clock line 
      while ( !PIR1bits.SSPIF );  // wait until ninth clock pulse received

      if ( ( !SSPSTATbits.R_W ) && ( !SSPSTATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
      {
        return ( -2 );            // terminate PutsI2C() function
      }
    }

  wrptr ++;                       // increment pointer

  }                               // continue data writes until null character

  return ( 0 );
}
