#include <p18cxxx.h>
#include <pwm.h>

#define __CONFIG3H 0x300005
char eccpmx; /* will be set iff bit 1 of __CONFIG3H is set */

/********************************************************************
*    Function Name:  SetOutputPWM1                                  *
*    Return Value:   void                                           *
*    Parameters:     outputconfig: holds PWM output configuration.  *
*                    These configurations are defined in pwm.h.     *
*                    outputmode: holds PWM output mode.             *
*                    These modes are defined in pwm.h.              *
*    Description:    This routine sets the PWM output config bits   *
*                    and mode bits for ECCP and configures the      *
*                    appropriate TRIS bits for output.              *
********************************************************************/
void SetOutputPWM1(unsigned char outputconfig, unsigned char outputmode)
{
  /* set P1M1 and P1M0 */
  CCP1CON = (CCP1CON | 0b11000000) & outputconfig;
  /* set CCP1M3, CCP1M2, CCP1M1, CCP1M0 */
  CCP1CON = (CCP1CON | 0b00001111) & outputmode;

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
#if defined(__18F1220) || defined(__18F1320)
      TRISBbits.TRISB3 = 0;
#else
      TRISCbits.TRISC2 = 0;
#endif
  }
  else if (IS_DUAL_PWM(outputconfig))
  {
#if defined(__18F1220) || defined(__18F1320)
      TRISBbits.TRISB3 = 0;
      TRISBbits.TRISB2 = 0;
#elif defined(__18F6585) || defined(__18F6680) || \
      defined(__18F8585) || defined(__18F8680) || \
      defined(__18F6525) || defined(__18F6621) || \
      defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
      defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
      defined(__18F6527) || defined(__18F6622) || \
      defined(__18F6627) || defined(__18F6722)
      TRISCbits.TRISC2 = 0;
      TRISEbits.TRISE6 = 0;
#elif defined(__18F8525) || defined(__18F8621) || \
      defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
      defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
      defined(__18F8527) || defined(__18F8622) || \
      defined(__18F8627) || defined(__18F8722)
      if (eccpmx)
      {
         TRISCbits.TRISC2 = 0;
         TRISEbits.TRISE6 = 0;
      }
      else
      {
         TRISCbits.TRISC2 = 0;
         TRISHbits.TRISH7 = 0;
      }
#else
  TRISCbits.TRISC2 = 0;
  TRISDbits.TRISD5 = 0;
#endif
  }
  else if (IS_QUAD_PWM(outputconfig))
  {
#if defined(__18F1220) || defined(__18F1320)
      TRISBbits.TRISB3 = 0;
      TRISBbits.TRISB2 = 0;
      TRISBbits.TRISB6 = 0;
      TRISBbits.TRISB7 = 0;
#elif defined(__18F6585) || defined(__18F6680) || \
      defined(__18F8585) || defined(__18F8680) || \
      defined(__18F6525) || defined(__18F6621) || \
      defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || \
      defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || \
      defined(__18F6527) || defined(__18F6622) || \
      defined(__18F6627) || defined(__18F6722)
      TRISCbits.TRISC2 = 0;
      TRISEbits.TRISE6 = 0;
      TRISEbits.TRISE5 = 0;
      TRISGbits.TRISG4 = 0;
#elif defined(__18F8525) || defined(__18F8621) || \
      defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || \
      defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || \
      defined(__18F8527) || defined(__18F8622) || \
      defined(__18F8627) || defined(__18F8722)
      if (eccpmx)
      {
         TRISCbits.TRISC2 = 0;
         TRISEbits.TRISE6 = 0;
         TRISEbits.TRISE5 = 0;
         TRISGbits.TRISG4 = 0;
      }
      else
      {
         TRISCbits.TRISC2 = 0;
         TRISGbits.TRISG4 = 0;
         TRISHbits.TRISH7 = 0;
         TRISHbits.TRISH6 = 0;
      }
#else
  TRISCbits.TRISC2 = 0;
  TRISDbits.TRISD5 = 0;
  TRISDbits.TRISD6 = 0;
  TRISDbits.TRISD7 = 0;
#endif
  }
}
