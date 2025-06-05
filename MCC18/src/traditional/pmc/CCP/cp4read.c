#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*   Function Name:   ReadCapture4                                   *
*   Return Value:    unsigned int: capture result                   *
*   Parameters:      void                                           *
*   Description:     This routine reads the CA4L and CA4H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadCapture4(void)
{
  union CapResult Cap;

  CapStatus.Cap4OVF = 0;   // Clear the overflow flag in the
                           // status variable

  if(PIR3bits.CCP4IF)      // If the overflow flag is set
    CapStatus.Cap4OVF = 1; // Set the overflow flag

  Cap.bc[0] = CCPR4L;      // Read CA4L into the lower byte
  Cap.bc[1] = CCPR4H;      // Read CA4H into the high byte

  return (Cap.lc);         // Return the int
}
