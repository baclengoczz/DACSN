#include <p18Cxxx.h>
#include <capture.h>

extern union capstatus CapStatus;

/********************************************************************
*   Function Name:   ReadCapture5                                   *
*   Return Value:    unsigned int: capture result                   *
*   Parameters:      void                                           *
*   Description:     This routine reads the CA5L and CA5H into      *
*                    the union Cap that is defined in global data   *
*                    space. The int result is then returned.        *
********************************************************************/
unsigned int ReadCapture5(void)
{
  union CapResult Cap;

  CapStatus.Cap5OVF = 0;   // Clear the overflow flag in the
                           // status variable

  if(PIR3bits.CCP5IF)      // If the overflow flag is set
    CapStatus.Cap5OVF = 1; // Set the overflow flag

  Cap.bc[0] = CCPR5L;      // Read CA5L into the lower byte
  Cap.bc[1] = CCPR5H;      // Read CA5H into the high byte

  return (Cap.lc);         // Return the int
}
