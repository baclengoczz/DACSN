/* $Id: t0write.c,v 1.2 2004/01/07 20:03:08 sealep Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  WriteTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer0                  *
*    Description:    This routine writes a 16-bit value to Timer0   *
*                    Timer0.                                        *
********************************************************************/
void WriteTimer0(unsigned int timer0)
{
  union Timers timer;

  timer.lt = timer0;    // Copy timer value into union

  TMR0H = timer.bt[1];  // Write high byte to Timer0
  TMR0L = timer.bt[0];  // Write low byte to Timer0
}

