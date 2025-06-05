#include <p18Cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  WriteTimer4                                    *
*    Return Value:   void                                           *
*    Parameters:     char: value to write to Timer4                 *
*    Description:    This routine writes a 8-bit value to Timer4.   *
********************************************************************/
#undef WriteTimer4
void WriteTimer4(unsigned char timer4)
{
  TMR4 = timer4;    // Write byte to Timer4
}
