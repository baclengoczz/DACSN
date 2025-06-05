/* $Id: adcopen.c,v 1.3 2005/06/28 19:04:36 nairnj Exp $ */
#include <p18cxxx.h>
#include <adc.h>

/********************************************************************
*    Function Name:  OpenADC                                        *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure A/D       *
*                    channel: channel selection                     *
*    Description:    This routine first resets the A/D registers    *
*                    to the POR state and then configures the       *
*                    A/D clock, interrupt, justification, vref      *
*                    source, and I/O.  It also sets the channel.    *
*    Notes:          The bit definitions for config and channel     *
*                    can be found in the adc16.h file.              *
********************************************************************/
#if !defined(ADC_INTERFACE_OLD) && \
    !defined(__18C601)  && !defined(__18C801)  && \
    !defined(__18C658)  && !defined(__18C858)  && \
    !defined(__18F6520) && !defined(__18F6620) && !defined(__18F6720) && \
    !defined(__18F8520) && !defined(__18F8620) && !defined(__18F8720)
void OpenADC( unsigned char config, 
              unsigned char config2, 
              unsigned char portconfig)
{
    // Reset A/D Registers to POR state
    ADCON0 = 0;
    ADCON1 = 0;
    ADCON2 = 0;

    ADCON0 = (config2 >> 1) & 0b00111100;     // Analog channel selection
    ADCON1 = portconfig;                      // Port configuration
#if defined(__18F1220) || defined(__18F1320)
    ADCON0 |= (config2 << 6) & 0b11000000;    // Voltage reference configuration
#else
    ADCON1 |= (config2 << 4) & 0b00110000;    // Voltage reference configuration
#endif
    ADCON2 = config & 0b10000000;             // Result format
    ADCON2 |= (config >> 4) & 0b00000111;     // Conversion clock select
    ADCON2 |= (config << 2) & 0b00111000;     // Acquisition time select

    if( config2 & 0b10000000 )   // Test the interrupt on/off
    {
      PIR1bits.ADIF = 0;         // Clear the A/D interrupt flag
      PIE1bits.ADIE = 1;         // Enable the A/D interrupt
      INTCONbits.PEIE = 1;       // Enable peripheral interrupts
    }

    ADCON0bits.ADON = 1;         // Enable the A/D
}

#else


void OpenADC( unsigned char config, unsigned char config2)
{
  // Reset A/D Registers to POR state
  ADCON0 = 0;
  ADCON1 = 0;
#ifndef ADC_INTERFACE_OLD
  ADCON2 = 0;
#endif

  // Configure ADCON registers as per config selections
#ifdef ADC_INTERFACE_OLD
  ADCON1 = config;             // Set the A/D clock source
  ADCON0 = (config<<2) & 0xC0; // Set the I/O selection
  ADCON0 |= config2 & 0b00111000;     // Set the channel
#else // New ADC interface
  ADCON0 = (config2 >> 1) & 0b00111100;    // Analog channel selection
  ADCON1 = ((config2 << 4) & 0b00110000) | // Voltage reference configuration
           (config & 0b00001111);          // Port configuration
  ADCON2 = (config & 0b10000000) |         // Result format
           ((config >> 4) & 0b00000111);   // Conversion clock select
#endif

  if( config2 & 0b10000000 )   // Test the interrupt on/off
  {
    PIR1bits.ADIF = 0;         // Clear the A/D interrupt flag
    PIE1bits.ADIE = 1;         // Enable the A/D interrupt
    INTCONbits.PEIE = 1;       // Enable peripheral interrupts
  }

  ADCON0bits.ADON = 1;         // Enable the A/D
}

#endif
