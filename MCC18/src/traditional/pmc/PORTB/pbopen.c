#include <p18cxxx.h>
#include <portb.h>

/* Configure PORTB interrupts,resistors */
void OpenPORTB(unsigned char config)
{
  //read portb to remove mismatch
  char temp;
  temp=PORTB;            //Read PORTB to remove mismatch/clear RBIF

  if(config&0x80)
    INTCONbits.RBIE=1;   //enable PORTB Interrupt on change

  if(config&0x01)
    DisablePullups();
  else
    EnablePullups();
}
