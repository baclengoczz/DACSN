#include <p18cxxx.h>
#include "usart.h"

#if defined(__18F6310) || defined(__18F6390) || \
    defined(__18F6410) || defined(__18F6490) || \
    defined(__18F8310) || defined(__18F8390) || \
    defined(__18F8410) || defined(__18F8490)
#define BAUDCON1 BAUDCTL1
#endif

/* **********************************************
 * Function Name: baud1USART                    *
 * Parameters: baudconfig                       *
 *             Configuration setting for the    *
 *             BAUDCON1 register.               *
 * Description: sets the configuration for the  *
 *              BAUDCON1 register.              *
 * **********************************************/

void baud1USART (unsigned char baudconfig)
{
    BAUDCON1 = baudconfig;
}
