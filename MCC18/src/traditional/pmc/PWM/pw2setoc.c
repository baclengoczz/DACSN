#include <p18cxxx.h>
#include <pwm.h>

#define __CONFIG3L 0x300004
#define __CONFIG3H 0x300005
char pmmode; /* will be set iff bit 0 AND bit 1 of __CONFIG3L are set */
char ccp2mx; /* will be set iff bit 0 of __CONFIG3H is set */

/********************************************************************
*    Function Name:  SetOutputPWM2                                  *
*    Return Value:   void                                           *
*    Parameters:     outputconfig: holds PWM output configuration.  *
*                    These configurations are defined in pwm.h.     *
*                    outputmode: holds PWM output mode.             *
*                    These modes are defined in pwm.h.              *
*    Description:    This routine sets the PWM output config bits   *
*                    and mode bits for ECCP2 and configures the     *
*                    appropriate TRIS bits for output.              *
********************************************************************/
void SetOutputPWM2(unsigned char outputconfig, unsigned char outputmode)
{
  /* set P1M1 and P1M0 */
  CCP2CON = (CCP2CON | 0b11000000) & outputconfig;
  /* set CCP1M3, CCP1M2, CCP1M1, CCP1M0 */
  CCP2CON = (CCP2CON | 0b00001111) & outputmode;
      

  if (((*(unsigned char far rom *)__CONFIG3H) & 0b00000001))
     ccp2mx=0xff;
  else
     ccp2mx = 0;

  if (((*(unsigned char far rom *)__CONFIG3L) & 0b00000011) == 0b00000011)
     pmmode=0xff;
  else
     pmmode = 0;

  /* Set the first TRIS. SINGLE_OUT uses the same register as the first */ 
  /* register for dual or quad                                          */
  if (ccp2mx)
  {
     TRISCbits.TRISC1 = 0;
  }
  else
  {
#if defined(__18F8527)  || defined(__18F8622)  || \
    defined(__18F8627)  || defined(__18F8722)  || \
    defined(__18F8525)  || defined(__18F8621)  || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) 
     if (pmmode)
     {
         TRISEbits.TRISE7 = 0;
     }
     else
     {
         TRISBbits.TRISB3 = 0;
     }
#elif defined(__18F24J10) || defined(__18F25J10) || \
      defined(__18F44J10) || defined(__18F45J10)
     TRISBbits.TRISB3 = 0;
#else
     TRISEbits.TRISE7 = 0;
#endif
  }
  if (IS_DUAL_PWM(outputconfig))
  {
     TRISEbits.TRISE2 = 0;
  }
  else if (IS_QUAD_PWM(outputconfig))
  {
     TRISEbits.TRISE2 = 0;
     TRISEbits.TRISE1 = 0;
     TRISEbits.TRISE0 = 0;
  }
}
