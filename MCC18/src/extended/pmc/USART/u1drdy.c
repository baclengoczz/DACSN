#include <p18cxxx.h>
#include <usart.h>

#if defined(__18F6525) || defined(__18F6621) || \
    defined(__18F8525) || defined(__18F8621)
#define RC1IF RCIF
#endif

/********************************************************************
*    Function Name:  DataRdy1USART                                   *
*    Return Value:   char                                           *
*    Parameters:     void                                           *
*    Description:    This routine checks the RCIF flag to see if    *
*                    any data has been received by the USART. It    *
*                    returns a 1 if data is available and a 0 if    *
*                    not.                                           *
********************************************************************/
#undef DataRdy1USART
char DataRdy1USART(void)
{
  if(PIR1bits.RC1IF)  // If RCIF is set
    return 1;  // Data is available, return TRUE
  else
    return 0;  // Data not available, return FALSE
}
