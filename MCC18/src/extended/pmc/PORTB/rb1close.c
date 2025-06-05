/* $Id: rb1close.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <portb.h>

/* Disable RB1INT interrupt */
#undef CloseRB1INT
void CloseRB1INT(void)
{
  INTCON3bits.INT1IE=0;     
}
