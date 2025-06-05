#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*   Function Name:   ReadCapture3                                   *
*   Return Value:    unsigned int: capture result                   *
*   Parameters:      void                                           *
*   Description:     This routine reads the CA3L and CA3H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadCapture3(void)
{
  union CapResult Cap;

  CapStatus.Cap3OVF = 0;   // Clear the overflow flag in the
                           // status variable

  if(PIR3bits.CCP3IF)      // If the overflow flag is set
    CapStatus.Cap3OVF = 1; // Set the overflow flag

  Cap.bc[0] = CCPR3L;      // Read CA3L into the lower byte
  Cap.bc[1] = CCPR3H;      // Read CA3H into the high byte

  return (Cap.lc);         // Return the int
}
