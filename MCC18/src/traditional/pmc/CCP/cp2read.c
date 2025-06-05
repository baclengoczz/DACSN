/* $Id: cp2read.c,v 1.2 2004/02/10 22:08:16 sealep Exp $ */

#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*   Function Name:   ReadCapture2                                   *
*   Return Value:    unsigned int: capture result                   *
*   Parameters:      void                                           *
*   Description:     This routine reads the CA2L and CA2H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadCapture2(void)
{
  union CapResult Cap;

  CapStatus.Cap2OVF = 0;   // Clear the overflow flag in the
                           // status variable

  if(PIR2bits.CCP2IF)      // If the overflow flag is set
    CapStatus.Cap2OVF = 1; // Set the overflow flag

  Cap.bc[0] = CCPR2L;      // Read CA2L into the lower byte
  Cap.bc[1] = CCPR2H;      // Read CA2H into the high byte

  return (Cap.lc);         // Return the int
}
