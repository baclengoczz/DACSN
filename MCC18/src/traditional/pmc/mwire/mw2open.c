/* $Id:  */
#include <p18cxxx.h>
#include <mwire.h>

/********************************************************************
*   Function Name:  OpenMwire2                                      *
*   Return Value:   void                                            *
*   Parameters:     SSP2 peripheral setup byte                      *
*   Description:    This function sets up the SSP2 module on a      * 
*                   PIC18CXXX core device for use with a Microchip  *
*                   Microwire EEPROM device.                        *
********************************************************************/
void OpenMwire2( unsigned char sync_mode )
{
  SSP2STAT &= 0x3F;                // power on state 
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode 

#if defined(__18F45J10) || defined(__18F44J10)
  TRISDbits.TRISD2 = 0;            // define SDO2 as output 
  TRISDbits.TRISD0 = 0;            // define clock as output   
  TRISDbits.TRISD1 = 1;            // define SDI2 as input 
#else
  TRISDbits.TRISD4 = 0;            // define SDO2 as output 
  TRISDbits.TRISD6 = 0;            // define clock as output   
  TRISDbits.TRISD5 = 1;            // define SDI2 as input 
#endif
  
  SSP2CON1 |= SSPENB;              // enable synchronous serial port 
}
