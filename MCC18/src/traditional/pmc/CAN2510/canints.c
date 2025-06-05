#include <spi.h>
#include <can2510.h>
#include "spi_can.h"

unsigned char CAN2510InterruptStatus( void )
{
    return( CAN2510ByteRead( CAN2510_REG_CANSTAT ) & 0x0E );         // Return the source of the interrupt(s)
}
