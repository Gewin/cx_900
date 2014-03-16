#include "iom128.h"
#include "ina90.h"


extern void trs0(unsigned char m,unsigned char n,unsigned char acc);

void disd(unsigned char m)
{/* unsigned char n;
  if(m==1)
  {
    for(n=0;n<3;n++) trs0(1,2,0x9e);              
  }
  else if(m==0)
  { 
    for(n=0;n<9;n++) trs0(1,2,0x80);
  } 
  else if(m==2) 
  {
    for(n=0;n<8;n++) trs0(1,2,0x9e);
  } */
} 

void disdian(unsigned char m)
{/* if(m==3)                        //display 3 black boxes
  {
    trs0(0,2,0xbf);
    trs0(0,2,0x40);
    disd(2);
    trs0(1,2,0x8c);
    trs0(0,2,0x4d);    //55
    disd(1);
    trs0(0,2,0x52);    //60
    disd(1);
    trs0(0,2,0x57);    //6b
    disd(1);
  } 
  else if(m==2)                   //display 2 black boxes
  {
    trs0(0,2,0xbf);
    trs0(0,2,0x40);
    disd(2);
    trs0(1,2,0x8c);
    trs0(0,2,0x4d);
    disd(0);
    trs0(0,2,0x52);
    disd(1);
    trs0(0,2,0x57);
    disd(1); 
  }
  else if(m==1)                        //displey 1 boxes
  {
    trs0(0,2,0xbf);
    trs0(0,2,0x40);
    disd(2);
    trs0(1,2,0x8c);
    trs0(0,2,0x4d);
    disd(0);
    trs0(0,2,0x52);
    disd(0);
    trs0(0,2,0x57);
    disd(1); 
  }
  else if(m==0)                       //display 0 boxes
  {
    trs0(0,2,0xbf);
    trs0(0,2,0x40);
    disd(2);
    trs0(1,2,0x8c);
    trs0(0,2,0x4d);
    disd(0);
    trs0(0,2,0x52);
    disd(0);
    trs0(0,2,0x57);
    disd(0); 
  }
  trs0(0,2,0xb8);  //ajust line  and column
  trs0(0,2,0x48); */
} 

