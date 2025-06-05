/* $Id: rb1open.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Enable RB1INT interrupts, falling edge/rising edge */
void OpenRB1INT(unsigned char config)
{
  INTCON3bits.INT1IF=0;

  if(config&0x02)          //Is rising edge used
    INTCON2bits.INTEDG1=1; //Yes, rising edge is used
  else
    INTCON2bits.INTEDG1=0; //No, falling edge is used

  if(config & ~PORTB_CHANGE_INT_OFF)
      INTCON3bits.INT1IE=1;
  else
      INTCON3bits.INT1IE=0;

  if(config & ~PORTB_PULLUPS_ON)
      INTCON2bits.RBPU=1;
  else
  {
      INTCON2bits.RBPU=0;
      TRISBbits.TRISB1=1;
  }

}
