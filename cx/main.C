#include "iom128.h"         		
#include "ina90.h"
#include<math.h>

#include "cx_common.h"
#include "keyanswer1.h"
#include "ADF4118.h"
#include "LCD_240160.h"
#include "usart.h"
#include "AD_RF.h"

void port_init(void)
{
	XMCRB=0x06;//pc2---pc7 as common I/O
	
	DDRB |= (1<<PB5) | (1<<PB7);
	PORTB &= ~((1<<PB5)|(1<<PB7));
	
	DDRC |= 0xfc;
	PORTC |= 0xf8;          //PC2  J5
	
	DDRD = 0xff;
	PORTD = PORTD & 0x1f;
	
	DDRE = 0x02;
	PORTE = 0x80;
	
	DDRG |= (1<<PG3) | (1<<PG4);    		//��ģ��Ŀ���λ
	PORTG &= ~((1<<PG3) | (1<<PG4));
	
	DDRF |= (1<<PF0);                         //J6
	DDRF &= ~((1<<PF1) | (1<<PF2) | (1<<PF3));
	PORTF &= ~(1<<PF0);
	
}
void Init_Timer()
{
	
	TCCR3A = 0X00;				//��ͨģʽ  64��Ƶ
	TCCR3B = 0X03;
	TCCR3C = 0X00;

	TCNT3H = 0X1E;				//���ö�ʱ����ֵ ��ʱ1s �Ƚ�׼ȷ    ��1E FF 1s�� ��8F 7F 0.5s��
	TCNT3L = 0XFF;

	ETIMSK = 0X04;				//����жϴ�
}

void main(void)
{
	MCUCR=0x80;
	port_init();
	
	
	//EL_EN; 
	delay(65535);//��λʱ��ϳ��ȴ�RA8822��λ���
	delay(65535);
	//EL_CLOSE; 
    //delay(65535);
	//delay(65535);
	//delay(20000);
	EL_CLOSE;   //�ر���
	lcd_init();
	//delay(50);
	lcd_clear();
	delay(10);
		
	display_img(0,0,main_interface1,1,30,160);
	
	display_freq(); 				//������ʾ467.7MHz
	
	display_img(15,26,key1[11],1,2,16);  		//-92
	display_img(17,26,key1[9],1,2,16);
	display_img(19,26,key1[2],1,2,16);
	display_hz(&chars_NPdBm[2],22,27,3);        //��λdBm
	
	display_img(15,55,E,1,10,16); //��Ƶ���� ����ʱΪE 
	display_img(15,86,E,1,10,16);//������� ����ΪE
	//display_img(13,86,key1[0],1,2,16);
	//display_img(15,86,key1[13],1,2,16);
	//display_img(17,86,key1[11],1,2,16);
	//display_img(19,86,key1[2],1,2,16);
	//display_img(21,86,key1[0],1,2,16);
	//display_img(23,86,key1[14],1,2,16);
	//display_hz(&chars_NPdBm[2],25,86,3);
	
	//display_img(25,1,MODE[1],1,5,14);
	display_hz(&Mode1[0],25,1,1);
	display_img(26,1,key1[15],1,2,16);
	display_hz(&Mode1[3],28,1,1);
	
	//delay(65535);
	//delay(65535);
	//delay(65535);
	//delay(65535);
	//delay(65535);
	//delay(30000);
	
	display_hz(&chars_1550nm[0],22,143,6);//�չ⹦��nm  
	EL_EN;  //������
	
	InitADF4118Gpio();
	init_usart0();
	Init_Timer();
	
	eeprom_write(EEP_UNIT_FG, 1);         //dw_flag  //��λ��־  1��ʾΪdbm 0��ʾΪW ��ʼΪdbm
	eeprom_write(EEP_DBVALUE, 92);
	eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);         //db_flag ��ʼΪ0����ʾ���ڵ�ֵΪ��
	eeprom_write(EEP_DB_CHANGE_RANGE, 1);         //key_flag ��־�Ӽ�10���ǼӼ�1
	eeprom_write(EEP_CURRENT_CUR_LOCATION, 1);         //flag_f ��¼��������
	eeprom_write(EEP_RF_ON_OFF_FG, 1);          //flagRFOnOff ��Ƶon/off��־
	eeprom_write(EEP_RF_FIRST_BIT, RF_DOWN_FIRST_BIT);
	eeprom_write(EEP_UP_DOWN_FG, RF_DOWN_SECOND_BIT);         
    eeprom_write(EEP_RF_THREAD_BIT, RF_DOWN_THREAD_BIT);
    eeprom_write(EEP_RF_FOURTH_BIT, RF_BIT_DEFAULT);
    eeprom_write(EEP_RF_FIFTH_BIT, RF_BIT_DEFAULT);
	eeprom_write(EEP_RECENT_FAR_FG,1);			//����Զ�˵ı�־ 1Ϊ���ˣ���ʼ״̬�� 0ΪԶ��
	
	putoutcode(92);
	InitADF4118(DOWN_GO_DEFAULT,REF_FRE);
	_SEI();
	
	//while(1)
	//{	
		//delay(65530);
		KeyAnswer(); 		
	//}
}

