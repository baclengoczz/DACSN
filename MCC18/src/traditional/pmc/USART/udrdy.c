/* $Id: udrdy.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <usart.h>

/********************************************************************
*    Function Name:  DataRdyUSART                                   *
*    Return Value:   char                                           *
*    Parameters:     void                                           *
*    Description:    This routine checks the RCIF flag to see if    *
*                    any data has been received by the USART. It    *
*                    returns a 1 if data is available and a 0 if    *
*                    not.                                           *
********************************************************************/
#undef DataRdyUSART
char DataRdyUSART(void)
{
  if(PIR1bits.RCIF)  // If RCIF is set
    return 1;  // Data is available, return TRUE
  else
    return 0;  // Data not available, return FALSE
}
