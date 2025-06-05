/* $Id: ep1read.c,v 1.1 2004/02/10 21:03:07 sealep Exp $ */
#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*    Function Name:  ReadECapture1                                  *
*    Return Value:   unsigned int: capture result                   *
*    Parameters:     void                                           *
*    Description:    This routine reads the CA1L and CA1H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadECapture1(void)
{
  union CapResult Cap;

  CapStatus.ECap1OVF = 0;    // Clear the overflow flag in the
                             // status variable

  if(PIR2bits.ECCP1IF)       // If the overflow flag is set
    CapStatus.ECap1OVF = 1;  // Set the overflow flag

  Cap.bc[0] = ECCPR1L;       // Read CA1L into the lower byte
  Cap.bc[1] = ECCPR1H;       // Read CA1H into the high byte

  return (Cap.lc);          // Return the int
}

