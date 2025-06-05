#include <p18cxxx.h>
#include "usart.h"

/* **********************************************
 * Function Name: baud2USART                    *
 * Parameters: baudconfig                       *
 *             Configuration setting for the    *
 *             BAUDCON2 register.               *
 * Description: sets the configuration for the  *
 *              BAUDCON2 register.              *
 * **********************************************/

void baud2USART (unsigned char baudconfig)
{
    BAUDCON2 = baudconfig;
}
