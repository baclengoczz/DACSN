#include "spi_can.h"
#include <can2510.h>


void CAN2510SetMode( unsigned char mode )
{
    CAN2510BitModify( CAN2510_REG_CANCTRL, 0xE0, mode );
}


