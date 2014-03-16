#include "iom128.h"
#include "ina90.h"

extern void trs0(unsigned char m,unsigned char n,unsigned char acc);
extern void WriteCur(unsigned char index,char step); 
void TransPl(void) ; 
void ChreTPl(void) 
{  unsigned char check;                                         //check if data unlocked y retransmit
 check=PINF;
  check=check&0x01;
  for(;check==1;)
  { TransPl(); 
    check=PINF;
    check=check&0x01; 
  }   
  DDRB=DDRB&0xfe;       
  PORTB=PORTB&0xf7;         
  DDRB=DDRB|0x10;                                                //N transmit pulse
  TCCR0=0x79;
  OCR0=0x70;  
}   

void  TransPl(void)  
{ 
   unsigned char i1,eewe;
   unsigned char data0,data1,data2; 
    
   DDRB=DDRB|0x41;
   PORTB=PORTB|0x40;
   eewe=0x02;
   for (;eewe==0x02;) eewe=EECR&0x02; 
   EEARH=0x00; EEARL=0x12;
   EECR=0x01; data0=EEDR;  
   eewe=0x02;
   for (;eewe==0x02;) eewe=EECR&0x02; 
   EEARH=0x00; EEARL=0x13;
   EECR=0x01; data1=EEDR;  
   eewe=0x02;
   for (;eewe==0x02;) eewe=EECR&0x02; 
   EEARH=0x00; EEARL=0x14;
   EECR=0x01; data2=EEDR; 

   DDRB=DDRB|0x01;
   PORTB=PORTB&0xfe;
   DDRB=DDRB|0x01;
   PORTB=PORTB|0x01; 
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   DDRB=DDRB|0x01;
   PORTB=PORTB&0xfe;
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
   for(i1=0;i1<40;i1++) ;   for(i1=0;i1<40;i1++) ;  for(i1=0;i1<40;i1++) ;  
  
   DDRB=DDRB&0xf7|0x08;
   PORTB=PORTB&0xf7; 
   
   DDRB=DDRB|0x02; 
   PORTB=PORTB&0xfd;
   
   DDRB=DDRB|0x04;
   PORTB=PORTB&0xfb;for(i1=0;i1<40;i1++) ;         
    
   DDRB=DDRB|0x02;     //N11  '0'
   PORTB=PORTB|0x02;for(i1=0;i1<40;i1++) ;    
  /////////////    
   DDRB=DDRB|0x02; 
   PORTB=PORTB&0xfd;  
   
   DDRB=DDRB|0x04;
   PORTB=PORTB|0x04;for(i1=0;i1<40;i1++) ;      
    
   DDRB=DDRB|0x06;  //N10  '1'
   PORTB=PORTB|0x06; for(i1=0;i1<40;i1++) ;  
 ///////////////  
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;    
   
   DDRB=DDRB|0x04;
   PORTB=PORTB&0xfb; for(i1=0;i1<40;i1++) ;  
   
   DDRB=DDRB|0x02; //N9  '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;   
   ////////////////
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;   
   
   DDRB=DDRB|0x04;
   PORTB=PORTB&0xfb;for(i1=0;i1<40;i1++) ;   
   
   DDRB=DDRB|0x02; //N8  '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;                
   /////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;  
   
   DDRB=DDRB|0x04;
   PORTB=(data0&0x08)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;                   
   
   DDRB=DDRB|0x02;  //N7  '1'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);
  //////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;   //for(i=0;i<50;i++);      
     
   DDRB=DDRB|0x04;
   PORTB=(data0&0x04)?(PORTB|0x04):(PORTB&0xfb);  for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                              
   
   DDRB=DDRB|0x02; //N6  '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;     //for(i=0;i<50;i++);        
  ///////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;    //for(i=0;i<50;i++);

   DDRB=DDRB|0x04;
   PORTB=(data0&0x02)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                           
   
   DDRB=DDRB|0x02; //N5  '0'
   PORTB=PORTB|0x02;for(i1=0;i1<40;i1++) ;         //for(i=0;i<50;i++);    
  ////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;     //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data0&0x01)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                               
    
   DDRB=DDRB|0x02;  //N4  '1'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;       //for(i=0;i<50;i++);
  //////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;     //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x80)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                               
  
   DDRB=DDRB|0x02; //N3 '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);
  ///////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;     //for(i=0;i<50;i++);
               
   DDRB=DDRB|0x04;
   PORTB=(data1&0x40)?(PORTB|0x04):(PORTB&0xfb);for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                             
   
   DDRB=DDRB|0x02; //N2 '0'
   PORTB=PORTB|0x02;for(i1=0;i1<40;i1++) ;        //for(i=0;i<50;i++);
  //////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x20)?(PORTB|0x04):(PORTB&0xfb);for(i1=0;i1<40;i1++) ;   //for(i=0;i<50;i++);                              
   
   DDRB=DDRB|0x02; //N1  '0'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);
   /////////////             
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;    //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x10)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;   //for(i=0;i<50;i++);                                
   
   DDRB=DDRB|0x02; //A7 '0'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);       
  ///////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x08)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++); 
      
   DDRB=DDRB|0x02;  //A6 '1'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;       //for(i=0;i<50;i++);
  //////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x04)?(PORTB|0x04):(PORTB&0xfb);for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);
               
   DDRB=DDRB|0x02;  //A5  '1'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;          //for(i=0;i<50;i++);
  //////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;       //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data1&0x02)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++); 
                                
   DDRB=DDRB|0x02; //A4 '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;        //for(i=0;i<50;i++);   
  ////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
    
   DDRB=DDRB|0x04;
   PORTB=(data1&0x01)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;   //for(i=0;i<50;i++);                            
   
   DDRB=DDRB|0x02; //A3 '0'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);           
  ////////////
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;     //for(i=0;i<50;i++);
    
   DDRB=DDRB|0x04;
   PORTB=(data2&0x80)?(PORTB|0x04):(PORTB&0xfb);  for(i1=0;i1<40;i1++) ;  //for(i=0;i<50;i++);   
    
   DDRB=DDRB|0x02; //A2 '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;         //for(i=0;i<50;i++);       
   ////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;       //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=(data2&0x40)?(PORTB|0x04):(PORTB&0xfb); for(i1=0;i1<40;i1++) ;    //for(i=0;i<50;i++);                             
   
   DDRB=DDRB|0x02; //A1 '0'
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);                 
  ///////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;     //for(i=0;i<50;i++);
    
   DDRB=DDRB|0x04;//'1'
   PORTB=PORTB|0x04; for(i1=0;i1<40;i1++) ;                             //for(i=0;i<50;i++);  
    
   DDRB=DDRB|0x02;  
   PORTB=PORTB|0x02; for(i1=0;i1<40;i1++) ;          //for(i=0;i<50;i++);
   //////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;        //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;//0///2002 7 24
   PORTB=PORTB&0xfb; for(i1=0;i1<40;i1++) ;                       //for(i=0;i<50;i++); 
    
   DDRB=DDRB|0x02; 
   PORTB=PORTB|0x02;   for(i1=0;i1<40;i1++) ;           //for(i=0;i<50;i++);  
   //////////////////
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;           //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=PORTB&0xfb;for(i1=0;i1<40;i1++) ; // for(i=0;i<50;i++);
   
   
   DDRB=DDRB|0x02; //'0'
   PORTB=PORTB|0x02;for(i1=0;i1<40;i1++) ;        //for(i=0;i<50;i++);            
  ///////////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
    
   DDRB=DDRB|0x04;
   PORTB=PORTB&0xfb; for(i1=0;i1<40;i1++) ;     //for(i=0;i<50;i++); 
   
   DDRB=DDRB|0x02; //'0'
   PORTB=PORTB|0x02;  for(i1=0;i1<40;i1++) ;      //for(i=0;i<50;i++);  
   /////////// 
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;      //for(i=0;i<50;i++);
   
   DDRB=DDRB|0x04;
   PORTB=PORTB|0x04; for(i1=0;i1<40;i1++) ; // for(i=0;i<50;i++);  
    
   DDRB=DDRB|0x02;  //CO '1'
   PORTB=PORTB|0x02;for(i1=0;i1<40;i1++) ;        //for(i=0;i<50;i++);
   //////////           
   DDRB=DDRB|0x02;
   PORTB=PORTB&0xfd;  //for(i=0;i<50;i++);
      
   DDRB=DDRB|0x08;
   PORTB=PORTB|0x08; for(i1=0;i1<40;i1++) ;   //for(i=0;i<50;i++);
     
   DDRB=DDRB|0x09;
   PORTB=PORTB&0xf6;  for(i1=0;i1<40;i1++) ; 
     
   DDRB=DDRB|0x10;
                                          //transmit pulse
   TCCR0=0x7a;
   OCR0=0x70; 
    for(i1=0;i1<100;i1++) ;
    for(i1=0;i1<100;i1++) ;
    for(i1=0;i1<100;i1++) ;
    for(i1=0;i1<100;i1++) ;
    for(i1=0;i1<100;i1++) ;
    for(i1=0;i1<100;i1++) ;  
  
}
 
 
     
void WPlddataEE(unsigned long speed, unsigned char method)    //get frequency and write data to EEPROM;
{ 
   unsigned char A,Nh,Nl,eewe=0x02,ainc,i,Result[3];
   unsigned long temp; 
   ainc=0x12; 
   temp=(speed*1024/12800);
   A=(unsigned char)(temp%64);
   temp=(temp-A)/64;    
   Nl=temp&0x00ff;
   Nh=(temp&0xff00)>>8;
   Result[0]=(Nh<<5)|(Nl>>3); 
   Result[1]=(Nl<<5)|(A>>2);
   Result[2]=((A<<6)|(0x30)|method); 
   for(i=0;i<3;i++)
   {    
    for(;eewe==0x02;) eewe=EECR&0x02;
    EEARH=0x00;
    EEARL=ainc;
    EEDR=Result[i];
    EECR=0x04;
    EECR=EECR|0x02;   
    ainc++;
    eewe=0x02;
   }  
   
}

