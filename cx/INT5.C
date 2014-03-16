#include "iom128.h"
#include "ina90.h" 
extern void disdian(unsigned char m);  
#pragma vector=INT5_vect
__interrupt void int5_interrupt(void) 

{
  disdian(1);  
  EIMSK=0xc0; 
}
