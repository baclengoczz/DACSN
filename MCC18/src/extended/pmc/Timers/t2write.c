/* $Id: t2write.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18Cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  WriteTimer2                                    *
*    Return Value:   void                                           *
*    Parameters:     char: value to write to Timer2                 *
*    Description:    This routine writes a 8-bit value to Timer2.   *
********************************************************************/
#undef WriteTimer2
void WriteTimer2(unsigned char timer2)
{
  TMR2 = timer2;    // Write byte to Timer2
}
