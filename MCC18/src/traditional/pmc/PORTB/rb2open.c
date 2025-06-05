/* $Id: rb2open.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Enable RB2 interrupts, falling edge/rising edge */
void OpenRB2INT(unsigned char config)
{
  INTCON3bits.INT2IF=0;
  if(config&0x02)            //Is rising edge used?
    INTCON2bits.INTEDG2=1;   //Yes, rising edge is used
  else
    INTCON2bits.INTEDG2=0;   //No, falling edge is used
  INTCON3bits.INT2IE=1;

  if(config & ~PORTB_CHANGE_INT_OFF)
      INTCON3bits.INT2IE=1;
  else
      INTCON3bits.INT2IE=0;

  if(config & ~PORTB_PULLUPS_ON)
  {
      INTCON2bits.RBPU=1;
  }
  else
  {
      INTCON2bits.RBPU=0;
      TRISBbits.TRISB2=1;
  }


}
