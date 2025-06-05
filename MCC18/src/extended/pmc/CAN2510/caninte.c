#include <spi.h>
#include <can2510.h>
#include "spi_can.h"


    /********************************************************************/
    /*                                                                  */
    /* The CANINTE register can be modified by the bit modify command   */
    /*                                                                  */
    /********************************************************************/

void CAN2510InterruptEnable( unsigned char interruptFlags )
{
    static unsigned char setting;

    /* Select what interrupts are enabled */
    setting = ~interruptFlags;
    CAN2510ByteWrite( CAN2510_REG_CANINTE, setting );

}
