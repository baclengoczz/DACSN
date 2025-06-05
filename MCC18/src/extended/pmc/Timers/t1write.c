/* $Id: t1write.c,v 1.2 2004/01/07 20:36:04 sealep Exp $ */
#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  WriteTimer1                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer1                  *
*    Description:    This routine writes a 16-bit value to Timer1.  *
********************************************************************/
void WriteTimer1(unsigned int timer1)
{
  union Timers timer;

  timer.lt = timer1;    // Save the 16-bit value in local

  TMR1H = timer.bt[1];  // Write high byte to Timer1 High byte
  TMR1L = timer.bt[0];  // Write low byte to Timer1 Low byte
}

