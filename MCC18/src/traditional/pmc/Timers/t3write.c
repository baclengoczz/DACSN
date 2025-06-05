/* $Id: t3write.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  WriteTimer3                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer3                  *
*    Description:    This routine writes a 16-bit value to Timer3   *
********************************************************************/
void WriteTimer3(unsigned int timer3)
{
  union Timers timer;

  timer.lt = timer3;    // Save value in local variable
  TMR3H = timer.bt[1];  // Write high byte to Timer3
  TMR3L = timer.bt[0];  // Write low byte to Timer3
}

