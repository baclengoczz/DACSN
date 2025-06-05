/* $Id: pulldis.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Disable PORTB resistors */
#undef DisablePullups
void DisablePullups(void)
{
  INTCON2bits.RBPU=1;
}
