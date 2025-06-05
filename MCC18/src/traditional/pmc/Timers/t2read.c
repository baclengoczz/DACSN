/* $Id: t2read.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  ReadTimer2                                     *
*    Return Value:   char: Timer2 8-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 8-bit value from        *
*                    Timer2.                                        *
********************************************************************/
unsigned char ReadTimer2(void)
{
  unsigned char timer;

  timer = TMR2;      // Read Timer2

  return (timer);    // Return 8-bits
}
