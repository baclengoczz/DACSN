/* $Id: t1read.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  ReadTimer1                                     *
*    Return Value:   char: Timer1 16-bit value                      *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer1.                                        *
********************************************************************/
unsigned int ReadTimer1(void)
{
  union Timers timer;

  timer.bt[0] = TMR1L;    // Read Lower byte
  timer.bt[1] = TMR1H;    // Read upper byte

  return (timer.lt);      // Return the 16-bit value
}
