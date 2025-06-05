/* $Id: setcsspi.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <sw_spi.h>

/********************************************************************
*       Function Name:  SetCSSWSPI                                  *
*       Return Value:   void                                        *
*       Parameters:     void                                        *
*       Description:    This routine sets the CS pin high.          *
********************************************************************/
void SetCSSWSPI(void)
{
        SW_CS_PIN = 1;                  // Set the CS pin high
}
