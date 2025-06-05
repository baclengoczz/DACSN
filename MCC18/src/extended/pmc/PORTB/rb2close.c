/* $Id: rb2close.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Disable RB2INT interrupt */
#undef CloseRB2INT
void CloseRB2INT(void)
{
  INTCON3bits.INT2IE=0;
}
