#include "iom128.h"
#include "ina90.h"  
extern void disdian(unsigned char m);

#pragma vector=INT4_vect
__interrupt void int4_interrupt(void) 
{ 
  disdian(2);
  EIMSK=0xe0;  
}



                       
