#include "iom128.h"
#include "ina90.h"

#include "ADF4118.h"
//#include"LCD_240160.h"

void InitADF4118Gpio()
{
	PORTB |= (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB6);// | (1<<4);
    DDRB |=  (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB6);// | (1<<4);	(1<<0) |
    
    PORTB &= ~(1<<PB0);
}


void Init_Timer0()
{
	TCCR0 = 0X1A;			//CTC模式	8分频 3.6864晶振
	
	OCR0 = 229;
}
void SetADF4118Reg(unsigned long int data)
{   
    uchar i,j;
    unsigned long int flag = (unsigned long int)1 << 20;
    flag = 0x100000;
    
    //1.Ready
    PORTB &= ~(1<<PB1);		//CLK
    PORTB &= ~(1<<PB3);		//LE
            
    //2.Set Data
    for(i=0; i<21; i++)
    {
        if(data & flag)
        {
            PORTB |= (1<<PB2);	//DATA
        }
        else
        {
            PORTB &= ~(1<<PB2);	//DATA
        }
        for(j=0; j<10; j++);
        PORTB |= (1<<PB1);		//CLK	
		//PORTB &= ~(1<<PB1);		//CLK
        for(j=0; j<10; j++);
		//PORTB |= (1<<PB1);
        PORTB &= ~(1<<PB1);		//CLK		
        flag >>= 1;
    }
    
    //3.LE
    PORTB |= (1<<PB3);		//LE
}

//Ref:参考频率 单位KHZ 
//Fvco:输出频率 单位KHZ
void InitADF4118(unsigned long int Fvco, uint Ref)
{
    unsigned long int RegValue, ACount,BCount;
    
    RegValue = ((unsigned long int)1<<7) | ((unsigned long int)1<<4) | 3;
    SetADF4118Reg(RegValue);
    RegValue = 12800 / Ref;
    RegValue <<= 2;
    SetADF4118Reg(RegValue);
	
    RegValue = Fvco / Ref;
    BCount = RegValue / 32;	//P = 32
    ACount = RegValue - BCount * 32;
    RegValue = ((unsigned long int)1<<20) | (BCount<<7) | (ACount<<2) | 1;
    SetADF4118Reg(RegValue);
	
	RegValue = ((unsigned long int)1<<7) | ((unsigned long int)1<<4) | 2;
    SetADF4118Reg(RegValue);	
    
	Init_Timer0();
}
