/* $Id: pbclose.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Disable PORTB interrupts, resistors */
#undef ClosePORTB
void ClosePORTB(void)
{
  INTCONbits.RBIE=0;  //Turn off PORTB interrupt on change
  DisablePullups();
}
