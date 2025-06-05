#include "spi_can.h"
#include <spi.h>
#include <can2510.h>


void CAN2510Reset( void )
{
    CAN2510Enable(  );                        // Enable SPI Communication to MCP2510
    while( WriteSPI(CAN2510_CMD_RESET) );
    CAN2510Disable(  );                       // Disable SPI Communication to MCP2510
}

