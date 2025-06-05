/* $Id: t0read.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  ReadTimer0                                     *
*    Return Value:   int: Timer0 16-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer0.                                        *
********************************************************************/
unsigned int ReadTimer0(void)
{
  union Timers timer;

  timer.bt[0] = TMR0L;  // Copy Timer0 low byte into union
  timer.bt[1] = TMR0H;  // Copy Timer0 high byte into union

  return (timer.lt);    // Return the int
}


