#include <p18cxxx.h>
#include <pwm.h>

#define __CONFIG3H 0x300005
char eccpmx; /* will be set iff bit 1 of __CONFIG3H is set */

/********************************************************************
*    Function Name:  SetOutputPWM3                                  *
*    Return Value:   void                                           *
*    Parameters:     outputconfig: holds PWM output configuration.  *
*                    These configurations are defined in pwm.h.     *
*                    outputmode: holds PWM output mode.             *
*                    These modes are defined in pwm.h.              *
*    Description:    This routine sets the PWM output config bits   *
*                    and mode bits for ECCP3 and configures the     *
*                    appropriate TRIS bits for output.              *
*                    Microcontroller mode is assumed.               *
********************************************************************/
void SetOutputPWM3(unsigned char outputconfig, unsigned char outputmode)
{
  /* set P1M1 and P1M0 */
  CCP3CON = (CCP3CON | 0b11000000) & outputconfig;
  /* set CCP1M3, CCP1M2, CCP1M1, CCP1M0 */
  CCP3CON = (CCP3CON | 0b00001111) & outputmode;
     
#if defined(__18F8525) || defined(__18F8621) || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
    defined(__18F8527) || defined(__18F8622) || \
    defined(__18F8627) || defined(__18F8722)
  if (((*(unsigned char far rom *)__CONFIG3H) & 0b00000010))
     eccpmx=0xff;
  else
     eccpmx = 0;
#endif

  if (SINGLE_OUT == outputconfig)
  {
     TRISGbits.TRISG0 = 0;
  }
  else if (IS_DUAL_PWM(outputconfig))
  {
#if defined(__18F6525)  || defined(__18F6621)  || \
    defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
    defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
    defined(__18F6527)  || defined(__18F6622)  || \
    defined(__18F6627)  || defined(__18F6722)
     TRISGbits.TRISG0 = 0;
     TRISEbits.TRISE4 = 0;
#endif
#if defined(__18F8525)  || defined(__18F8621)  || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
    defined(__18F8527)  || defined(__18F8622)  || \
    defined(__18F8627)  || defined(__18F8722)
     if (eccpmx)
     {
        TRISGbits.TRISG0 = 0;
        TRISEbits.TRISE4 = 0;
     }
     else
     {
        TRISGbits.TRISG0 = 0;
        TRISHbits.TRISH5 = 0;
     }
#endif
  }
  else if (IS_QUAD_PWM(outputconfig))
  {
#if defined(__18F6525)  || defined(__18F6621)  || \
    defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
    defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
    defined(__18F6527)  || defined(__18F6622)  || \
    defined(__18F6627)  || defined(__18F6722) 
     TRISGbits.TRISG0 = 0;
     TRISEbits.TRISE4 = 0;
     TRISEbits.TRISE3 = 0;
     TRISGbits.TRISG3 = 0;
#endif
#if defined(__18F8525)  || defined(__18F8621)  || \
    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
    defined(__18F8527)  || defined(__18F8622)  || \
    defined(__18F8627)  || defined(__18F8722)
     if (eccpmx)
     {
        TRISGbits.TRISG0 = 0;
        TRISEbits.TRISE4 = 0;
        TRISEbits.TRISE3 = 0;
        TRISGbits.TRISG3 = 0;
     }
     else
     {
        TRISGbits.TRISG0 = 0;
        TRISGbits.TRISG3 = 0;
        TRISHbits.TRISH5 = 0;
        TRISHbits.TRISH4 = 0;
     }
#endif
  }
}
