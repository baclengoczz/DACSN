/* $Id: statrst.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <reset.h>

/********************************************************************
*       Function Name:  StatusReset                                 *
*       Return Value:   char                                        *
*       Parameters:     void                                        *
*       Description:    This routine resets the POR and BOR bits.   *
********************************************************************/
void StatusReset(void)
{
        RCONbits.NOT_POR = 1;                   // Reset POR bit
        // If BOR enabled
#if defined(BOR_ENABLED)
        RCONbits.NOT_BOR = 1;                   // Reset BOR bit
#endif
}

