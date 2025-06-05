/* $Id: islvd.c,v 1.1 2003/12/09 22:52:09 GrosbaJ Exp $ */
#include <p18cxxx.h>
#include <reset.h>

/********************************************************************
*       Function Name:  isLVD                                       *
*       Return Value:   char                                        *
*       Parameters:     void                                        *
*       Description:    This routine determines if a low voltage    *
*                       condition occurred.                         *
*********************************************************************/
char isLVD()
{
   if(PIR2bits.LVDIF)
      return 1;
   else
      return 0;
}
