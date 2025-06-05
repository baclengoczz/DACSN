#include "spi_can.h"
#include <spi.h>
#include <can2510.h>


unsigned char CAN2510ByteRead( unsigned char addr )
{
    unsigned char ReturnValue;

    CAN2510Enable(  );                        // Enable SPI Communication to MCP2510
    while( WriteSPI(CAN2510_CMD_READ) );
    while( WriteSPI(addr) );
    ReturnValue = ReadSPI();
    CAN2510Disable(  );                       // Disable SPI Communication to MCP2510
    return( ReturnValue );
}

