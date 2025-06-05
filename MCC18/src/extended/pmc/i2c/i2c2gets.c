/* $Id:  */
#include <p18cxxx.h>
#include <i2c.h>


/********************************************************************
*     Function Name:    getsI2C2                                    *
*     Return Value:     error condition status                      *
*     Parameters:       address of read string storage location     *
*                       length of string bytes to read              *
*     Description:      This routine reads a predetemined string    *
*                       length in from the I2C2 bus. The routine is *
*                       developed for the Master mode. The bus ACK  *
*                       condition is generated within this routine. *
********************************************************************/
unsigned char getsI2C2( unsigned char *rdptr, unsigned char length )
{
    while ( length -- )            // perform getcI2C2() for 'length' number of bytes
    {
      *rdptr++ = getcI2C2();       // save byte received
      while ( SSP2CON2bits.RCEN ); // check that receive sequence is over    

      if ( PIR3bits.BCL2IF )       // test for bus collision
      {
        return ( -1 );             // return with Bus Collision error 
      }

      if ( length )                // test if 'length' bytes have been read
      {
        SSP2CON2bits.ACKDT = 0;    // set acknowledge bit state for ACK        
        SSP2CON2bits.ACKEN = 1;    // initiate bus acknowledge sequence
        while ( SSP2CON2bits.ACKEN ); // wait until ACK sequence is over 
      } 
    }
    return ( 0 );                  // last byte received so don't send ACK      
}
