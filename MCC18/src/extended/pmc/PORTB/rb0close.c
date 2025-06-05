/* $Id: rb0close.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Disable RB0INT interrupt */
#undef CloseRB0INT
void CloseRB0INT(void)
{
  INTCONbits.INT0IE=0;
}
