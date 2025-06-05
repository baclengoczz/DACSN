#include <p18cxxx.h>
#include <timers.h>

/********************************************************************
*    Function Name:  ReadTimer4                                     *
*    Return Value:   char: Timer4 8-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine returns the 8-bit value of        *
*                    Timer4.                                        *
********************************************************************/
unsigned char ReadTimer4(void)
{
  return (TMR4);    // Return 8-bits 
}
