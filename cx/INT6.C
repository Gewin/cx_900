#include "iom128.h"
#include "ina90.h" 
unsigned char ming,count,time_flag; 
extern void disdian(unsigned char m);
#pragma vector=INT6_vect
__interrupt void int6_interrupt(void) 

{   unsigned char m,n,i,j;
//SREG=0x00;   
EIMSK=0x80;
TIMSK=0;
disdian(0);

  for(m=0;m<2;m++)         //feng ming xiang 
  { 
   for(j=0;j<3;j++)
   {
     for(n=0;n<0xfe;n++)
     {
      DDRB=DDRB|0x20;     
      PORTB=PORTB|0x20;  
      for(i=0;i<0x24;i++)  ;  
      DDRB=DDRB|0x20;     
      PORTB=PORTB&0xdf; 
      for(i=0;i<0x24;i++)  ; 
     }
   } 
    

   for(n=0;n<0x88;n++)    
   {
     DDRB=DDRB|0x20;     
     PORTB=PORTB&0xdf;
     for(i=0;i<0xfe;i++)  ;
     for(i=0;i<0xfe;i++)  ;
   }
  }
  TIMSK=0x41; 
  TCCR0=7;
  TCNT0=0x37; 
  time_flag=0;
  ming=0; 
  count=0;
}  


#pragma vector=TIMER0_OVF_vect
__interrupt void timer0_ovf_interrupt(void)         //low volt warning and volt off
{ unsigned char m,n,i,j;
  //SREG=0x00;                
  if((count==0)&&(time_flag>=40)) 
  { if(ming<2)
    {for(m=0;m<2;m++)         //feng ming xiang 
     { for(j=0;j<3;j++)
       {
         for(n=0;n<0xfe;n++)
         { DDRB=DDRB|0x20;     
           PORTB=PORTB|0x20;  
           for(i=0;i<0x2d;i++)  ;   
           DDRB=DDRB&0xdf|0x20;     
           PORTB=PORTB&0xdf; 
           for(i=0;i<0x2d;i++)  ;  
         }
       } 
       for(n=0;n<0x88;n++)    
       { DDRB=DDRB|0x20;     
         PORTB=PORTB&0xdf;
         for(i=0;i<0xfe;i++)  ;for(i=0;i<0xfe;i++)  ;
         for(i=0;i<0xfe;i++)  ;
       }
     } 
     ming++;     
     time_flag=0; 
     TCCR0=7;
     TCNT0=0x37; 
   }     
     else 
     { count=1;
       TCCR0=7;
       TCNT0=0x37;
       time_flag=0;
     }
   }
   else if((count==0)&&(time_flag<40))
   { time_flag++;
     TCCR0=7;
     TCNT0=0x37; 
   } 
 
    
   else if(count==1) 
   { if(time_flag>=80)
      {  DDRB=DDRB|0x80;
         PORTB=PORTB|0x80; 
      }
     else 
     { time_flag++;     
       TCCR0=7;
       TCNT0=0x37; } 
   }    
} 
 
 
 


     
     
     
     
      
