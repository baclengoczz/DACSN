#include <p18cxxx.h>
#include <pwm.h>

#define __CONFIG3H 0x300005
#define __ECCPMX 1  /* bit 1 of __CONFIG3H */
static char eccpmx; /* will be set iff bit 1 of __CONFIG3H is set */

/********************************************************************
*    Function Name:  SetOutputEPWM1                                 *
*    Return Value:   void                                           *
*    Parameters:     outputconfig: holds PWM output configuration.  *
*                    These configurations are defined in pwm.h.     *
*                    outputmode: holds PWM output mode.             *
*                    These modes are defined in pwm.h.              *
*    Description:    This routine sets the PWM output config bits   *
*                    and mode bits for ECCP and configures the      *
*                    appropriate TRIS bits for output.              *
********************************************************************/
void SetOutputEPWM1(unsigned char outputconfig, unsigned char outputmode)
{
  // set P1M1 and P1M0
  ECCP1CON = (ECCP1CON | 0b11000000) & outputconfig;
  // set CCP1M3, CCP1M2, CCP1M1, CCP1M0
  ECCP1CON = (ECCP1CON | 0b00001111) & outputmode;

  if (SINGLE_OUT == outputconfig)
  {
      TRISDbits.TRISD4 = 0;
  }
  else if (IS_DUAL_PWM(outputconfig))
  {
      TRISDbits.TRISD4 = 0;
      TRISDbits.TRISD5 = 0;
  }
  else if (IS_QUAD_PWM(outputconfig))
  {
      TRISDbits.TRISD4 = 0;
      TRISDbits.TRISD5 = 0;
      TRISDbits.TRISD6 = 0;
      TRISDbits.TRISD7 = 0;
  }
}
