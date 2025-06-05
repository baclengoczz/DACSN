/* $Id: pullen.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Enable PORTB resistors */
#undef EnablePullups
void EnablePullups(void)
{
  INTCON2bits.RBPU=0;        
}
