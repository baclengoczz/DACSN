#include "spi_can.h"
#include <can2510.h>


unsigned char  CAN2510ReadMode( void )
{
    unsigned char mode;

    mode = CAN2510ByteRead( CAN2510_REG_CANSTAT );
    return ( mode & 0xE0 );                         // Mask off the OPMODE2:OPMODE0 bits and return this value
}                                                   // This indicates the MCP2510s current mode

