/* $Id: rb0open.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Enable RB0INT interrupt, falling edge/rising edge */
void OpenRB0INT(unsigned char config)
{
  INTCONbits.INT0IF=0;

  if(config&0x02)
    INTCON2bits.INTEDG0=1;
  else
    INTCON2bits.INTEDG0=0;

  if(config & ~PORTB_CHANGE_INT_OFF)
      INTCONbits.INT0IE=1;
  else
      INTCONbits.INT0IE=0;

  if(config & ~PORTB_PULLUPS_ON)
  {
      INTCON2bits.RBPU=1;
   }
  else
  {
      INTCON2bits.RBPU=0;
      TRISBbits.TRISB0=1;
  }

}
