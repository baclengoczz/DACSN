/* $Id: t3read.c,v 1.2 2004/03/24 16:26:04 sealep Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  ReadTimer3                                     *
*    Return Value:   int: Timer3 16-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer3.                                        *
********************************************************************/
unsigned int ReadTimer3(void)
{
  union Timers timer;

  timer.bt[0] = TMR3L;   // Read low byte of Timer3
  timer.bt[1] = TMR3H;   // Read high byte of Timer3

  return (timer.lt);     // Return 16-bit value
}
