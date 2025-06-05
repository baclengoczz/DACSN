#include <spi.h>
#include "spi_can.h"
#include <can2510.h>


void InitSPI( unsigned char SPI_syncMode,
              unsigned char SPI_busMode,
              unsigned char SPI_smpPhase )
{
    static unsigned char setting;

    /* Initialize SPI with user selected settings */
    OpenSPI( SPI_syncMode, SPI_busMode, SPI_smpPhase );


}
