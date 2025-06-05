#include <p18cxxx.h>
#include "usart.h"

#if defined(__18F1220) || defined(__18F1320)
#define BAUDCON BAUDCTL
#endif

/* **********************************************
 * Function Name: baudUSART                     *
 * Parameters: baudconfig                       *
 *             Configuration setting for the    *
 *             BAUDCTL register.                *
 * Description: sets the configuration for the  *
 *              BAUDCTL register.               *
 * **********************************************/

void baudUSART (unsigned char baudconfig)
{
    BAUDCON = baudconfig;
}
