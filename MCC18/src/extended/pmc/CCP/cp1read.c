/* $Id: cp1read.c,v 1.2 2004/02/10 21:04:04 sealep Exp $ */
#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*    Function Name:  ReadCapture1                                   *
*    Return Value:   unsigned int: capture result                   *
*    Parameters:     void                                           *
*    Description:    This routine reads the CA1L and CA1H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadCapture1(void)
{
  union CapResult Cap;

  CapStatus.Cap1OVF = 0;    // Clear the overflow flag in the
                            // status variable

  if(PIR1bits.CCP1IF)       // If the overflow flag is set
    CapStatus.Cap1OVF = 1;  // Set the overflow flag

  Cap.bc[0] = CCPR1L;       // Read CA1L into the lower byte
  Cap.bc[1] = CCPR1H;       // Read CA1H into the high byte

  return (Cap.lc);          // Return the int
}

