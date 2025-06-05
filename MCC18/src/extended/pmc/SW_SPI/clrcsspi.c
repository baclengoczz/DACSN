/* $Id: clrcsspi.c,v 1.1 2003/12/09 22:43:28 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <sw_spi.h>

/********************************************************************
*       Function Name:  ClearCSSWSPI                                *
*       Return Value:   void                                        *
*       Parameters:     void                                        *
*       Description:    This routine sets the CS pin low.           *
********************************************************************/
void ClearCSSWSPI(void)
{
        SW_CS_PIN = 0;                  // Clear the CS pin
}

