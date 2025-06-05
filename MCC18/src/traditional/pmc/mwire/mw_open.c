/* $Id: mw_open.c,v 1.1 2004/10/06 22:50:12 curtiss Exp $ */
#include <p18cxxx.h>
#include <mwire.h>

#if defined(__18F2331) || defined(__18F2431) || \
    defined(__18F4331) || defined(__18F4431)
#define SSPCON1 SSPCON
#define SSPCON1bits SSPCONbits
#endif

/********************************************************************
*   Function Name:  OpenMwire                                       *
*   Return Value:   void                                            *
*   Parameters:     SSP peripheral setup byte                       *
*   Description:    This function sets up the SSP module on a       * 
*                   PIC18CXXX core device for use with a Microchip  *
*                   Microwire EEPROM device.                        *
********************************************************************/
void OpenMwire( unsigned char sync_mode )
{
  SSPSTAT &= 0x3F;                // power on state 
  SSPCON1 = 0x00;                 // power on state
  SSPCON1 |= sync_mode;           // select serial mode 

#if defined(__18F2455) || defined(__18F2550) || \
    defined(__18F4455) || defined(__18F4550)
  TRISCbits.TRISC7 = 0;           // define SDO as output 
  TRISBbits.TRISB1 = 0;           // define clock as output   
  TRISBbits.TRISB0 = 1;           // define SDI as input 
#else
  TRISCbits.TRISC5 = 0;           // define SDO as output 
  TRISCbits.TRISC3 = 0;           // define clock as output   
  TRISCbits.TRISC4 = 1;           // define SDI as input 
#endif
  
  SSPCON1 |= SSPENB;              // enable synchronous serial port 
}
