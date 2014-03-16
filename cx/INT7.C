#include "iom128.h"
#include "ina90.h"

unsigned char dian,dianc;
extern void disdian(unsigned char m);
#pragma vector=INT7_vect
__interrupt void int7_interrupt(void)
{ 
  //SREG=0x00; 
  disdian(0);  
  dian=0; 
  dianc=0;
  TIMSK=0x40;
  EIMSK=0x00; 
}
