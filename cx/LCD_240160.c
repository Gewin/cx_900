#include "iom128.h"
#include "ina90.h"

#include"LCD_240160.h"

uint  p_add_dis_chu=0xfcff; 
/***************************************************
������void delay(uint t)
���ܣ���ʱ���뼶
******************************************************/
void delay(unsigned volatile int t)
{
	while(t--);
}
void eeprom_write(unsigned int add,uchar data)
{
	uchar eewe;
	eewe=0x02;
	for(;eewe==0x02;) 
	eewe=EECR&0x02;  
	EEARH=(uchar)(add>>8);
	EEARL=(uchar)add;
	EEDR=data;  
	EECR=0x04;
	EECR=EECR|0x02; 
}
uchar eeprom_read(unsigned int add)
{
    uchar data,eewe=0x02;
    for (;eewe==0x02;) 
      eewe=EECR&0x02; 
    EEARH=(uchar)(add>>8); 
    EEARL=(uchar)add;
    EECR=0x01; 
    data=EEDR;
    return data;
}

/**************************************************
������void lcd_cmdwrite (uchar cmdx , uchar cmdy)
���ܣ���ָ���ļĴ���д������
********************************************************/
void lcd_cmdwrite (uchar cmdx , uchar cmdy)
{
	uchar *p;
	p_add_dis_chu=p_add_dis_chu&0xfcff;
	p=(uchar *)p_add_dis_chu; 
	*p=cmdx;
	p_add_dis_chu=0xffff;
	p_add_dis_chu=p_add_dis_chu&0xfcff;
	p=(uchar *)p_add_dis_chu; 
	*p=cmdy;
	p_add_dis_chu=0xffff;
}
/*************************************************
������void lcd_datawrite(uchar datax)
���ܣ�д����ʾ������
*****************************************************/
void lcd_datawrite(uchar datax)
{
	uchar *p;
	p_add_dis_chu=p_add_dis_chu&0xfcff|0x0200;
	p=(uchar *)p_add_dis_chu; 
	*p=datax;
	p_add_dis_chu=0xfdff;
}
/********************************************************
������void lcd_init()
���ܣ���ʼ���öຯ��
********************************************************/
void lcd_init(void)
{
	lcd_cmdwrite(0x00,0xCd);//WLCR,
	lcd_cmdwrite(0x01,0xf2);//MISC
	lcd_cmdwrite(0x02,0x10);//APSR
	lcd_cmdwrite(0x03,0x80);//ADSR
	lcd_cmdwrite(0x10,0x2b);//WCCR//0x2b�������壬0x3b����
	lcd_cmdwrite(0x11,0x22);//CHLD
	lcd_cmdwrite(0x12,0x91);//MAMR
	lcd_cmdwrite(0x20,  29);//AWRR ��
	lcd_cmdwrite(0x30, 0x9f);//AWBR ��//127
	lcd_cmdwrite(0x40,0x00);
	lcd_cmdwrite(0x50,0x00);
	lcd_cmdwrite(0x21,  29);//��	
	lcd_cmdwrite(0x31, 0x9f);//��//127
	lcd_cmdwrite(0x41,0x00);
	lcd_cmdwrite(0x51,0x00);
	lcd_cmdwrite(0x60,0x00);
	lcd_cmdwrite(0x61,0x00);
	lcd_cmdwrite(0x70,0x00);
	lcd_cmdwrite(0x71,0x00);
	lcd_cmdwrite(0x72,0x9F);
	lcd_cmdwrite(0x80,0x33);
	lcd_cmdwrite(0x81,0x00);
	lcd_cmdwrite(0x91,0x00);
	lcd_cmdwrite(0x90,0x0C);
	lcd_cmdwrite(0xA0,0x11);
	lcd_cmdwrite(0xA1,0x00);
	lcd_cmdwrite(0xA2,0x00);
	lcd_cmdwrite(0xA3,0x00);
	lcd_cmdwrite(0xB0,0x1d);
	lcd_cmdwrite(0xB1,0x9f);
	lcd_cmdwrite(0xC0,0x00);
	lcd_cmdwrite(0xC1,0x0A);
	lcd_cmdwrite(0xC8,0x80);
	lcd_cmdwrite(0xC9,0x80);
	lcd_cmdwrite(0xCA,0x00);
	lcd_cmdwrite(0xD0,0x16);
	lcd_cmdwrite(0xE0,0x00);
	lcd_cmdwrite(0xF0,0xA0);
	lcd_cmdwrite(0xF1,0x0F);
}
/***********************************************
������void display_hz(uchar *text��uchar x,y)
���ܣ���ָ���ĵط���ʾ�ַ�������
******************************************************/
void display_hz(uchar __flash *text,uchar x,uchar y,uchar count)
{ ///x:0-30 y:0-160 
	uchar i;
	lcd_cmdwrite(0x00,0xcd); //����ģʽ
	lcd_cmdwrite(0x60,x);
	lcd_cmdwrite(0x70,y);
	for(i=0;i<count;i++)
	{
		lcd_datawrite(*text);
		++text;  
	}
}
/*******************************************
������void display_img(uchar code *p)
���ܣ���ʾһ��ͼ��
*********************************************/
void display_img(uchar col,uchar row,uchar __flash *p,uchar c,uchar width,uchar high)
{//width=ʵ�ʿ���/8
	uchar i,j;
	uint s=0;
	lcd_cmdwrite(0x00,0xc5);
	for(i=0;i<high;i++)  //
	{
		lcd_cmdwrite(0x00,0xc5);
		lcd_cmdwrite(0x60,col);//��
		lcd_cmdwrite(0x70,row++); //�� //������ʼ��
		for(j=0;j<width;j++)  //24064��30,160128��20
		{    
	 		if(c==1)
	 		{
				lcd_datawrite(p[s++]);
	  		}
	  		else if(c==0)
	  		{//����
				lcd_datawrite(~p[s++]);			
	  		}
	  		else if(c==0xff)
	  		{//����
				lcd_datawrite(0);
	  		}
		}	
	}     
}

void display_wdian(uchar s,uchar n)
{
	uchar i,j;
	lcd_cmdwrite(0x00,0xc5);
	for(i=0;i<160/n;i++)
	{
		for(j=0;j<30;j++)
		{
			lcd_datawrite(s);
			if(n==2)
				lcd_datawrite(s);
			if(n==4)
			{
				lcd_datawrite(s);
				lcd_datawrite(s);
				lcd_datawrite(s);
			}
		}
		s=~s;
	}
}

/*****************************************************
������void lcd_clear()
���ܣ�����ڴ�
********************************************************/
void lcd_clear(void)
{
	display_img(0,0,main_interface1,0xff,30,160);
}

uchar __flash main_interface1[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0x03,0x00,0xC0,0xC1,0x86,0x00,
0x06,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0x03,0x00,0xDF,0xE1,0x87,0x00,0x06,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x0F,0xC3,0x03,0xF3,0x07,0xFD,0x80,0xC6,0x60,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x0C,0xC3,0x03,0xCF,0xE3,0x18,0xC0,0xC6,0x60,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x0F,0xFF,0xC3,0xCC,0x63,0xFF,0xF0,0xC6,0x60,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0C,
0xC3,0x07,0xFF,0x66,0xC0,0x00,0xC6,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0F,0xF3,0x00,
0xCF,0x67,0xFE,0x60,0xFF,0xE0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0C,0xDB,0x03,0xCF,0x60,
0xF7,0xE0,0x06,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x1F,0xDB,0x03,0xFF,0x60,0xFF,0xE0,
0xC6,0x30,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xC3,0x06,0xFF,0x60,0xF7,0xE0,0xC6,0x30,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x03,0xC3,0x00,0x6F,0x67,0xFF,0xE0,0xC6,0x30,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x06,0xC3,0x00,0xC7,0x83,0xF7,0xE0,0xC6,0x30,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x0F,0xCF,0x01,0x8C,0xE0,0xF6,0x60,0xFF,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x19,
0x86,0x07,0x38,0x60,0xFE,0xE0,0x00,0x30,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x03,0x03,0x00,0xC0,0xC0,0x03,0x00,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,
0x03,0x00,0xDF,0xE0,0x03,0x00,0x06,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0F,0xC3,0x03,
0xF3,0x07,0xF3,0x01,0xFF,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0C,0xC3,0x03,0xCF,0xE0,
0xC3,0x60,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0F,0xFF,0xC3,0xCC,0x60,0xDF,0xF1,
0x9B,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0C,0xC3,0x07,0xFF,0x60,0xC3,0x60,0xFE,0xC0,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x0F,0xF3,0x00,0xCF,0x60,0xC3,0x60,0x6C,0xC0,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x0C,0xDB,0x03,0xCF,0x60,0xC3,0x60,0xDB,0x60,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x1F,0xDB,0x03,0xFF,0x60,0xC6,0x61,0xBF,0xB0,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,
0xC3,0x06,0xFF,0x60,0xF6,0x60,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x03,0xC3,0x00,
0x6F,0x67,0x8C,0x63,0xFF,0xF8,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x06,0xC3,0x00,0xC7,0x83,
0x18,0x60,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0F,0xCF,0x01,0x8C,0xE0,0x33,0xC0,
0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x19,0x86,0x07,0x38,0x60,0x61,0x80,0x0C,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x01,0x98,0x00,0x60,0x03,0x0C,0x00,0x0C,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x0D,0x8C,0x00,0x60,0x03,0x0E,0x00,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0D,
0x8C,0x06,0x63,0x0F,0xFB,0x01,0x8C,0xC0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x19,0x83,0x03,
0x66,0x06,0x31,0x81,0x8C,0xC0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x1F,0xFF,0x81,0xEC,0x07,
0xFF,0xE1,0x8C,0xC0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x60,0x0D,0x80,0x01,
0x8C,0xC0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xFE,0x1F,0xFF,0xCF,0xFC,0xC1,0xFF,0xC0,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x03,0xC6,0x00,0xD8,0x01,0xEF,0xC0,0x0C,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x03,0xCC,0x00,0xD8,0x01,0xFF,0xC1,0x8C,0x60,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x06,0x78,0x00,0xD8,0x01,0xEF,0xC1,0x8C,0x60,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x06,
0x30,0x01,0x98,0x0F,0xFF,0xC1,0x8C,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x0C,0x78,0x03,
0x18,0xC7,0xEF,0xC1,0x8C,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x19,0xCF,0x86,0x18,0xC1,
0xEC,0xC1,0xFF,0xE0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x3F,0x03,0x1C,0x0F,0xC1,0xFD,0xC0,
0x00,0x60,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x06,0xC0,0x00,0xC0,0x00,0x0C,0x00,0x30,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x06,0xC0,0x00,0xC0,0x00,0x0C,0x00,0x19,0x80,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x06,
0xC6,0x0C,0xC6,0x1F,0xCC,0x07,0xFF,0xC0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x36,0xFF,0x06,
0xCC,0x03,0x0D,0x80,0x30,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x37,0x8C,0x03,0xD8,0x03,
0x7F,0xC6,0x6D,0x80,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x37,0x8C,0x00,0xC0,0x03,0x0D,0x83,
0xFB,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x37,0xCC,0x3F,0xFF,0x83,0x0D,0x81,0xB3,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x36,0xD8,0x01,0xB0,0x03,0x0D,0x83,0x6D,0x80,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x3E,0x78,0x01,0xB0,0x03,0x19,0x86,0xFE,0xC0,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x3E,0x30,0x01,0xB0,0x03,0xD9,0x80,0x30,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x36,
0x30,0x03,0x30,0x1E,0x31,0x8F,0xFF,0xE0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x06,0x78,0x06,
0x31,0x8C,0x61,0x80,0x30,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x06,0xCF,0x0C,0x31,0x80,
0xCF,0x00,0x30,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x07,0x86,0x38,0x1F,0x81,0x86,0x00,
0x30,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uchar __flash chars_F[]={"F:EdBmnmMHz"};
uchar __flash chars_freq[]={"F: 467.700MHz"};
uchar __flash chars_1310nm[]={"1310nm"};
uchar __flash chars_1550nm[]={"1550nm"};
uchar __flash chars_dBm[]={"- 21 dBm"};

uchar __flash space[]={"                  "};
uchar __flash dot[]={"."};
uchar __flash chars_MHz[]={"MHz"};
uchar __flash chars_NPdBm[]={"-+dBmW"};

uchar __flash Mode1[]={"J--X"};
uchar __flash Mode2[]={"J--S"};
uchar __flash Mode3[]={"Y--X"};
uchar __flash Mode4[]={"Y--S"};
uchar __flash Mode5[]={"Z--J"};

uchar __flash battery[5][48]={
								{0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xFF,0x80,
								 0x00,0x00,0x01,0xFF,0xFF,0x80,0x00,0x00,0x01,0xFF,0xFF,0x80,0x00,0x00,0x01,0xFF,
								 0xFF,0x80,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00},
								{0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xFF,0x8F,
								 0x00,0x00,0x01,0xFF,0xFF,0x8F,0x00,0x00,0x01,0xFF,0xFF,0x8F,0x00,0x00,0x01,0xFF,
								 0xFF,0x8F,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00},
								{0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xFF,0x8F,
								 0x3C,0x00,0x01,0xFF,0xFF,0x8F,0x3C,0x00,0x01,0xFF,0xFF,0x8F,0x3C,0x00,0x01,0xFF,
								 0xFF,0x8F,0x3C,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00},
								{0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xFF,0x8F,
								 0x3C,0xF0,0x01,0xFF,0xFF,0x8F,0x3C,0xF0,0x01,0xFF,0xFF,0x8F,0x3C,0xF0,0x01,0xFF,
								 0xFF,0x8F,0x3C,0xF0,0x01,0xFF,0xF8,0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x00,0x00},
								{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
							};

uchar __flash E[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFE,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uchar __flash key[]={"0123456789ABCDEFGHIJKLMN"};
uchar __flash key1[17][32]={
								 {0x00,0x00,0x07,0xC0,0x0C,0x60,0x0C,0x60,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,      //0
								  0x18,0x30,0x18,0x30,0x18,0x30,0x0C,0x60,0x0C,0x60,0x07,0xC0,0x00,0x00,0x00,0x00}, 
								 {0x00,0x00,0x01,0x80,0x07,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
								  0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x07,0xE0,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x00,0x30,0x00,0x60,0x00,0x60,      //2
								  0x00,0xC0,0x01,0x80,0x03,0x00,0x06,0x30,0x0C,0x30,0x0F,0xF0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x07,0xC0,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x00,0xC0,0x03,0xC0,0x00,0x60,      //3
								  0x00,0x30,0x00,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x60,0x07,0xC0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0xC0,0x01,0xC0,0x03,0xC0,0x06,0xC0,0x06,0xC0,0x0C,0xC0,0x0C,0xC0,      //4
								  0x18,0xC0,0x1F,0xF0,0x00,0xC0,0x00,0xC0,0x00,0xC0,0x03,0xF0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x0F,0xF0,0x0C,0x00,0x0C,0x00,0x0C,0x00,0x0C,0x00,0x0F,0xC0,0x0E,0x60,      //5
								  0x00,0x30,0x00,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x60,0x07,0xC0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x03,0xE0,0x06,0x30,0x0C,0x30,0x0C,0x00,0x18,0x00,0x1F,0xC0,0x1C,0x60,      //6
								  0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x0C,0x60,0x07,0xC0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x0F,0xF0,0x0C,0x60,0x0C,0x60,0x00,0xC0,0x00,0xC0,0x01,0x80,0x01,0x80,      //7
								  0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x07,0xC0,0x0C,0x60,0x18,0x30,0x18,0x30,0x18,0x30,0x0C,0x60,0x07,0xC0,      //8
								  0x0C,0x60,0x18,0x30,0x18,0x30,0x18,0x30,0x0C,0x60,0x07,0xC0,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x07,0xC0,0x0C,0x60,0x18,0x30,0x18,0x30,0x18,0x30,0x18,0x30,0x0C,0x70,      //9
								  0x07,0xF0,0x00,0x30,0x00,0x60,0x18,0x60,0x18,0xC0,0x0F,0x80,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x1F,0xF0,0x01,0x00,      //+
								  0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},                                     
								 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,      //-
								  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //.
								  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0x18,0x00,0x30,0x00,0x60,0x00,0x60,0x00,0xC0,0x00,0xC0,0x00,0xC0,
								  0x00,0xC0,0x00,0xC0,0x00,0xC0,0x00,0x60,0x00,0x60,0x00,0x30,0x00,0x18,0x00,0x00},
								 {0x00,0x00,0x0C,0x00,0x06,0x00,0x03,0x00,0x03,0x00,0x01,0x80,0x01,0x80,0x01,0x80,       //)
								  0x01,0x80,0x01,0x80,0x01,0x80,0x03,0x00,0x03,0x00,0x06,0x00,0x0C,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      //-.-
								  0x00,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
								 {0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x30,0x0D,0xB0,0x0D,0xB0,0x0D,0xB0,0x0D,0xB0,      //w
0x0F,0xF0,0x0F,0xF0,0x07,0xE0,0x07,0xE0,0x06,0x60,0x06,0x60,0x06,0x60,0x00,0x00}
								 
                             };

void display_freq(void)
{  
	//uchar bit1,bit2,bit3,bit4;
	
	//bit1=eeprom_read(0x21);
	//bit2=eeprom_read(0x22);
	//bit3=eeprom_read(0x23);
	//bit4=eeprom_read(0x24);
	display_hz(&chars_F[0],12,1,2);
	display_hz(&key[9],14,1,1);
	display_hz(&key[3],15,1,1);
	display_hz(&key[2],16,1,1);
	display_hz(&dot[0],17,1,1);
	display_hz(&key[0],18,1,1);
	display_hz(&key[0],19,1,1);
	display_hz(&key[0],20,1,1);
	display_hz(&chars_MHz[0],21,1,3);
	//display_hz(&key[0],25,1,1);
	//switch(bit4)
	//{
	//case 0:
		//display_hz(&key[0],23,1,1);
		//display_hz(&key[0],24,1,1);
		/*break;
	case 2:
		display_hz(&key[2],23,1,1);
		display_hz(&key[5],24,1,1);
		break;
	case 5:
		display_hz(&key[5],23,1,1);
		display_hz(&key[0],24,1,1);
		break;
	case 7:
		display_hz(&key[7],23,1,1);
		display_hz(&key[5],24,1,1);
		break;
	default:
		break;  
	}*/
	
}
void display1_freq(void)
{  
	//uchar bit1,bit2,bit3,bit4;
	
	//bit1=eeprom_read(0x21);
	//bit2=eeprom_read(0x22);
	//bit3=eeprom_read(0x23);
	//bit4=eeprom_read(0x24);
	display_hz(&chars_F[0],12,1,2);
	display_hz(&key[8],14,1,1);
	display_hz(&key[8],15,1,1);
	display_hz(&key[7],16,1,1);
	display_hz(&dot[0],17,1,1);
	display_hz(&key[0],18,1,1);
	display_hz(&key[0],19,1,1);
	display_hz(&key[0],20,1,1);
	display_hz(&chars_MHz[0],21,1,3);
}