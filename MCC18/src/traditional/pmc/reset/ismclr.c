/* $Id: ismclr.c,v 1.2 2005/06/28 19:04:36 nairnj Exp $ */
#include <p18cxxx.h>
#include <reset.h>

#if defined(__18C242)  || defined(__18C252)  || \
    defined(__18C442)  || defined(__18C452)  || \
    defined(__18C658)  || defined(__18C858)  || \
    defined(__18F1220) || defined(__18F1320) || \
    defined(__18F242)  || defined(__18F2439) || \
    defined(__18F2455) || defined(__18F248)  || \
    defined(__18F252)  || defined(__18F2539) || \
    defined(__18F2550) || defined(__18F258)  || \
    defined(__18F442)  || defined(__18F4439) || \
    defined(__18F4455) || defined(__18F448)  || \
    defined(__18F452)  || defined(__18F4539) || \
    defined(__18F4550) || defined(__18F458)
#define STKOVF STKFUL
#endif

/********************************************************************
*       Function Name:  isMCLR                                      *
*       Return Value:   char                                        *
*       Parameters:     void                                        *
*       Description:    This routine checks to see if a MCLR induced*
*                       reset occurred.                             *
********************************************************************/

char isMCLR(void)
{
        if(RCONbits.NOT_POR)                    // If POR is set
        {
                if(RCONbits.NOT_PD)             // If NOT_PD is set
                {
                // If BOR is enabled
#if defined(BOR_ENABLED)
                        if(RCONbits.NOT_BOR)    // If BOR is set
                        {
                        // If BOR and WDT are enabled
#if defined(WDT_ENABLED)
                                if(!RCONbits.NOT_TO)    // If TO is cleared
                                        return 0x00;// Return FALSE
#endif
                                if(!RCONbits.NOT_RI)    // If RI is cleared
                                        return 0x00;// Return FALSE

#if defined(STVR_ENABLED)
                                if(STKPTRbits.STKOVF)    // If STKOVF is set
                                        return 0x00;// Return FALSE

                                if(STKPTRbits.STKUNF)    // If STKUNF is set
                                        return 0x00;// Return FALSE
#endif

                                return 1;       // Else return TRUE
                        }
                        return 0x00;            // Else return FALSE
        // BOR is disabled
#else
        // If WDT is enabled
#if defined(WDT_ENABLED)
                        if(!RCONbits.NOT_TO)    // If TO is cleared
                                return 0x00;    // Return FALSE

                        if(!RCONbits.NOT_RI)    // If RI is cleared
                                return 0x00;// Return FALSE
#if defined(STVR_ENABLED)

                        if(STKPTRbits.STKOVF)    // If STKOVF is set
                                return 0x00;// Return FALSE

                        if(STKPTRbits.STKUNF)    // If STKUNF is set
                                return 0x00;// Return FALSE
#endif
                         return 1;               // Else return TRUE
        // BOR and WDT are disabled
#else
                        if(!RCONbits.NOT_RI)    // If RI is cleared
                                return 0x00;// Return FALSE

#if defined(STVR_ENABLED)
                        if(STKPTRbits.STKOVF)    // If STKOVF is set
                                return 0x00;// Return FALSE

                        if(STKPTRbits.STKUNF)    // If STKUNF is set
                                return 0x00;// Return FALSE
#endif
                        return 1;               // Else return TRUE
#endif
#endif
                }
                return 0x00;                    // Else return FALSE
        }
        return 0x00;                            // Else return FALSE
}

