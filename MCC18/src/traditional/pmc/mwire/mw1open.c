/* $Id:  */
#include <p18cxxx.h>
#include <mwire.h>

/********************************************************************
*   Function Name:  OpenMwire1                                      *
*   Return Value:   void                                            *
*   Parameters:     SSP1 peripheral setup byte                      *
*   Description:    This function sets up the SSP1 module on a      * 
*                   PIC18CXXX core device for use with a Microchip  *
*                   Microwire EEPROM device.                        *
********************************************************************/
void OpenMwire1( unsigned char sync_mode )
{
  SSP1STAT &= 0x3F;                // power on state 
  SSP1CON1 = 0x00;                 // power on state
  SSP1CON1 |= sync_mode;           // select serial mode 

  TRISCbits.TRISC5 = 0;            // define SDO1 as output 
  TRISCbits.TRISC3 = 0;            // define clock as output   
  TRISCbits.TRISC4 = 1;            // define SDI1 as input 
  
  SSP1CON1 |= SSPENB;              // enable synchronous serial port 
}
