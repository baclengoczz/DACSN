/* $Id: spi_clos.c,v 1.1 2004/10/06 23:00:51 curtiss Exp $ */
#include <p18cxxx.h>
#include <spi.h>

/********************************************************************
*     Function Name:    CloseSPI                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      This function disables the SSP module. Pin  *
*                       I/O returns under the control of the port   *
*                       registers.                                  *
********************************************************************/
#undef CloseSPI
void CloseSPI( void )
{
  SSPCON1 &= 0xDF;                // disable synchronous serial port
}
