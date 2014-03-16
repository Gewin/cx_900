#include "iom128.h"
#include "ina90.h" 
#include"math.h"

#include "cx_common.h"
#include "keyanswer1.h"
#include "ADF4118.h"
#include "LCD_240160.h" 
#include "usart.h"
#include "AD_RF.h"

uint advalue[18];  //存储AD转换后数据的数组，采集18次，去除最高和最低取平均值
uint j,k,temp;   //AD转换 对采集到得10个数据进行排序时用的变量，冒泡排序
float FinData;
uint ADC_rel;

volatile char RF_input[10]={0X7E,0X06,0X00,0X5A,0X00,0X01,0X00,0X40,0XA3,0X7E};		
volatile char RF_output[10]={0X7E,0X06,0X00,0X5B,0X00,0X01,0X00,0XF4,0XD5,0X7E};
volatile char Light_input[13]={0X7E,0X06,0X00,0XF0,0X00,0X04,0X6E,0X02,0X00,0X00,0XFE,0X69,0X7E}; //串口通讯时用到的命令
volatile char Light_output[10]={0X7E,0X06,0X00,0X66,0X00,0X01,0X00,0X9B,0XC0,0X7E};

volatile uchar flagRecentFar;						  //近端机（1）还是远端机（0）   0x25
volatile uint relayStatus,charge_flag,geshu_flag = 0;   //geshu_flag是充电时 显示电池个数的控制变量

volatile uint Light_number = 1;         //保存计算机过位数的变量

volatile uchar keyvalue=0,keynumber=0;    //按键时用到的变量

volatile uchar bit0,bit1,bit2,bit3,bit4;

volatile uchar refresh_flag = 1,refresh1_flag = 1,refresh2_flag = 0;           //普通状态和中继状态间转换时用到的变量

volatile uchar flagPressMhzKey = 0;

volatile uchar no1bit,no2bit,dbvalue;   //按下上下按键时使用的变量
volatile uchar db_flag,flagPressKeyTime,key_flag,flagRFOnOff;     //正负标志变量 0表示现在的值为负 1为正  flagPressKeyTime是当前显示位置的标志量
                                                           //key_flag标志加减10（=0）还是加减1（=1） flagRFOnOff 是标志射频on/off
                                                            //dw_flag w和dBm单位的标志1（dBm）  AD_flag 测得的射频值的正负(1为正)

void WriteCur(uchar index,char step)//记录按下当前按键应对应哪个位置
{     
  if(step>=10)
    index=index-(step/10);
  else 
    index=index+step;
  eeprom_write(EEP_CURRENT_CUR_LOCATION,index);
}
           
void KeyUp(void)                //等待按键抬起
{
	uchar pine,i,portc,key_result=0;
	while (key_result==0)
	{
		
		keyvalue=0;
		PORTC=PORTC&0x07|0xf0;
		for(i=0;i<5;i++)
		{
			portc=PORTC&0xf8;
			pine=PINE&0x7c; 
			switch (pine)
			{
			case 0x78:
				keyvalue=((4-i)*5+1);
				i=5;     
				break;
			case 0x74:
				keyvalue=((4-i)*5+2);
				i=5;
				break;
			case 0x6c:
				keyvalue=((4-i)*5+3);
				i=5;
				break;
			case 0x5c:
				keyvalue=((4-i)*5+4);
				i=5;
				break; 
			case 0x3c:
				keyvalue=((4-i)*5+5);
				i=5;
				break;  
			default:
				break;
			}
			if(portc==0xf0)
			{//L1
				PORTC=PORTC&0x07|0xe8;
			}
			else if(portc==0xe8)
			{//L2
				PORTC=PORTC&0x07|0xd8;
			}
			else if(portc==0xd8)
			{//L3
				PORTC=PORTC&0x07|0xb8;
			}
			else if(portc==0xb8)
			{//L4
				PORTC=PORTC&0x07|0x78;
			}
		}
		if (keyvalue==0) 
			key_result=1;
	}
}

float ADport(uchar port)
{
	ADMUX = port;
	ADMUX &= 0x3f; //外部基准源
	ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0); //先使能ADC,32分频
	
	ADC_rel = 0;
	for(int i=0;i<18;i++)  //每次转换采样10次，取其平均值
	{
		ADCSRA|=_BV(ADSC);  //开始转换
		while(!(ADCSRA&_BV(ADIF)));      //转换结束跳出
		advalue[i]=ADCL;
		advalue[i]=advalue[i]+ADCH*256; 
		if(i==17)                  //收集18次后进入if语句
		{
			for(j=0;j<18;j++)           //把收集到得18个数据排序
			{
				for(k=0;k<17-j;k++)
				{
					if(advalue[k]>advalue[k+1])
					{
						temp=advalue[k];
						advalue[k]=advalue[k+1];
						advalue[k+1]=temp;
					}
				}
			}
			for(i=1;i<17;i++)            //去除首尾数据求平均值
			{
				ADC_rel+=advalue[i];
			}
			FinData=ADC_rel>>4;      
		}						 
	} 
	FinData=(unsigned long)FinData*4500/1024; 

	return FinData;
}

void clearbit(uchar index)
{
	if(index==1)
  	{ 
    	display_hz(&space[0],14,1,5);
  	}
  	if(index==2)
  	{ 
    	display_hz(&space[0],15,1,9);
  	} 
  	if(index==3)
  	{ 
    	display_hz(&space[0],16,1,8);
  	}
  	if(index==5)
  	{ 
    	display_hz(&space[0],18,1,5);
  	}
  	if(index==6)
  	{ 
    	display_hz(&space[0],19,1,5);
  	} 
}

uchar KeyScan(void)             //键盘扫描
{
	uchar pine,i,portc;
  
  //while (result==0)                   
  //{
    PORTC=PORTC&0x07|0xf0;
    for(i=0;i<5;i++)
    {
      portc=PORTC&0xf8;
      pine=PINE&0x7c; 
      switch (pine)
      {
      case 0x78:
        keyvalue=((4-i)*5+1);
        i=5;     
        break;
      case 0x74:
        keyvalue=((4-i)*5+2);
        i=5;
        break;
      case 0x6c:
        keyvalue=((4-i)*5+3);
        i=5;
        break;
      case 0x5c:
        keyvalue=((4-i)*5+4);
        i=5;
        break; 
      case 0x3c:
        keyvalue=((4-i)*5+5);
        i=5;
        break;  
      default:
		  keyvalue = 0;
        break;
      }
      if(portc==0xf0)
      {//L1
        PORTC=PORTC&0x07|0xe8;
      }
      else if(portc==0xe8)
      {//L2
        PORTC=PORTC&0x07|0xd8;
      }
      else if(portc==0xd8)
      {//L3
        PORTC=PORTC&0x07|0xb8;
      }
      else if(portc==0xb8)
      {//L4
        PORTC=PORTC&0x07|0x78;
      }
    }
    if(keyvalue == 21){keynumber = 1; bgon_off = 0;}
    else if(keyvalue == 22){keynumber = 5; bgon_off = 0;}
    else if(keyvalue == 23){keynumber = 9; bgon_off = 0;}  
    else if(keyvalue == 24){keynumber = KEY_LEFT; bgon_off = 0;}
    else if(keyvalue == 25){keynumber = KEY_RF; bgon_off = 0;}
    else if(keyvalue==16){keynumber=2;bgon_off = 0;}
    else if(keyvalue==17){keynumber=6;bgon_off = 0;}
    else if(keyvalue==18){keynumber=0;bgon_off = 0;}
    else if(keyvalue==19){keynumber=KEY_UP;bgon_off = 0;}
    else if(keyvalue==20){keynumber=KEY_BEIGUANG;bgon_off = 0;}
    else if(keyvalue==11){keynumber=3;bgon_off = 0;}
    else if(keyvalue==12){keynumber=7;bgon_off = 0;}
    else if(keyvalue==13){keynumber=KEY_C;bgon_off = 0;}
    else if(keyvalue==14){keynumber=KEY_RIGHT;bgon_off = 0;}
    else if(keyvalue==15){keynumber=KEY_DOWNGO;bgon_off = 0;}
    else if(keyvalue==6){keynumber=4;bgon_off = 0;}
    else if(keyvalue==7){keynumber=8;bgon_off = 0;}
    else if(keyvalue==8){keynumber=KEY_MHz;bgon_off = 0;}
    else if(keyvalue==9){keynumber=KEY_DOWN;bgon_off = 0;}
    else if(keyvalue==10){keynumber=KEY_UPGO;bgon_off = 0;}
    else if(keyvalue==1){keynumber=KEY_RECENT;bgon_off = 0;}
    else if(keyvalue==2){keynumber=KEY_FAR;bgon_off = 0;}
    else if(keyvalue==3){keynumber=KEY_dBm_W;bgon_off = 0;}
	else
	{
		keynumber = 30;
	}

 // } 
   
  return keynumber;
}
void KeyAnswer(void)
{
	float a;
	int b;
	uint temp, temp1;
 	uchar bitvalue0 = bit0, bitvalue1 = bit1, bitvalue2 = bit2, bitvalue3 = bit3, bitvalue4 = bit4, portd, tempValue = 0; 
 	unsigned long result = 0,no1value = 0, no2value = 0,no3value = 0,no4value = 0;
	unsigned long result1 = DOWN_GO_DEFAULT;
 	while(1)
 	{
		if(beiguang_off == 3)
		{
			EL_CLOSE; 
		}
		relayStatus = GPIO_F_RELAY & PINF;  //中继(1)  or  非中继(0)
		charge_flag = GPIO_E_CHARGE & PINE;  //是否在充电
		if(charge_flag == 0)
		{
			if(geshu_flag == 0)
			{
				display_img(12, 148, battery[0], 1, 6, 8);
				geshu_flag = 1;
			}
			else if(geshu_flag == 1)
			{
				display_img(12, 148, battery[1], 1, 6, 8);
				geshu_flag = 2;
			}
			else if(geshu_flag == 2)
			{
				display_img(12, 148, battery[2], 1, 6, 8);
				geshu_flag = 3;
			}
			else if(geshu_flag == 3)
			{
				display_img(12, 148, battery[3], 1, 6, 8);
				geshu_flag = 0;
			}
			if(relayStatus == 0)
			{
				delay(65000);            //中继状态和非中继状态的延时时间不同
                delay(15000);
			}
			else
			{
				delay(50000);
			}
		}
		else
		{
			if(flag_ADRF == 1)
			{
				Battery_acc = ADport(3);              //电池电量显示
				Battery_Dis();
			}
		}
		if(bgon_off == 60)                            //关背光
		{
			EL_CLOSE;
		}
		if(bgon_off == 180)                           //关射频
		{
			flagPressKeyTime=eeprom_read(EEP_CURRENT_CUR_LOCATION);
		    if(flagPressKeyTime>1)
		    {
			   eeprom_write(EEP_CURRENT_CUR_LOCATION,1);
		    }
		  	eeprom_write(EEP_RF_ON_OFF_FG,0);
		  	TX_UNEN;
		  	display_hz(&space[0],14,1,10);
		  	display_hz(&space[0],12,25,12);
		  	display_img(15,25,E,1,10,16); //射频输出置为E 
		  	CUT_RF_POWER;
			refresh_flag = 1;
		}
		if(bgon_off >= 480)                             //关机
		{
			PORTB |= (1<<PB7);
		}
		
		if(flag_ADRF == 1)                              //射频功率测量
		{
			RF_acc = ADport(2);
			RF_Dis();
		}		
		if(relayStatus == 0)                                //非中继状态
		{
			if(refresh2_flag == 1)            //由中继变到非中继时
			{
				display_freq(); 				//开机显示下行频率
				display_img(15, 26, key1[11], 1, 2, 16);  		//-92
				display_img(17, 26, key1[9], 1, 2, 16);
				display_img(19, 26, key1[2], 1, 2, 16);
				display_hz(&chars_NPdBm[2], 22, 26, 3);        //单位dBm
                eeprom_write(EEP_DBVALUE, 92);
				
				eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
				eeprom_write(EEP_DB_CHANGE_RANGE, 1);
                eeprom_write(EEP_CURRENT_CUR_LOCATION, 1);                  //位置标志
				eeprom_write(EEP_UP_DOWN_FG, DOWN_GO_MARK);
				eeprom_write(EEP_RF_ON_OFF_FG, 1);
				eeprom_write(EEP_RECENT_FAR_FG, 1);//近端机
				putoutcode(92);
				TX_EN;
				InitADF4118(DOWN_GO_DEFAULT, REF_FRE);
				eeprom_write(EEP_RF_FIRST_BIT, RF_DOWN_FIRST_BIT);
				eeprom_write(EEP_UP_DOWN_FG, RF_DOWN_SECOND_BIT);
			    eeprom_write(EEP_RF_THREAD_BIT, RF_DOWN_THREAD_BIT);
			    eeprom_write(EEP_RF_FOURTH_BIT, RF_BIT_DEFAULT);
			    eeprom_write(EEP_RF_FIFTH_BIT, RF_BIT_DEFAULT);
				result1 = DOWN_GO_DEFAULT;
				refresh2_flag = 0;	
				bgon_off = 0;                     //Reset Beiguang timer
			}
			if(refresh_flag == 1)
			{
				flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);                       //近端机or远端机标志位
				flagUpDown = eeprom_read(EEP_UP_DOWN_FG);                   //上行or下行标志位
				flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);                       //射频开or关标志位
				
				if(flagRecentFar == RECENT_MACHINE && flagUpDown == DOWN_GO_MARK)              //J-X
				{
#if 0				
					if(flagRFOnOff == 1)
					{
						display_img(15,26,key1[11],1,2,16);  		//-92
						display_img(17,26,key1[9],1,2,16);
						display_img(19,26,key1[2],1,2,16);
						display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
						eeprom_write(EEP_DBVALUE,92);
					}
#endif					
					display_hz(&Mode1[0], 25, 1, 1);
					display_img(26, 1, key1[15], 1, 2, 16);
					display_hz(&Mode1[3], 28, 1, 1);       //j-x
					display_hz(&chars_1550nm[0], 22, 143, 6);
					PORTG &= ~((1<<PG3) | (1<<PG4));
                                        
                    //PORTG |= (1<<PG3);
					//PORTG &= ~(1<<PG4);
                                        
					PORTC &= ~(1<<PC2);    //J5
					PORTF |= (1<<PF0);     //J6
					display_hz(&space[0], 13, 86, 16);
					display_img(15, 86, E, 1, 10, 16);//发光输出 
					
					if(flagPressMhzKey == 1)
					{
						InitADF4118(result1, REF_FRE);
						flagPressMhzKey = 0;
					}
					else
					{
						InitADF4118(DOWN_GO_DEFAULT, REF_FRE);
					}
				}
				else if(flagRecentFar == 1 && flagUpDown == UP_GO_MARK)         //J-S
				{
                    display_hz(&space[0],12,25,12);
		  			display_img(15,25,E,1,10,16); //射频输出置为E 
                    putoutcode(92);
					//TX_EN;
					display_hz(&Mode2[0], 25, 1, 1);
					display_img(26, 1, key1[15], 1, 2, 16);
					display_hz(&Mode2[3], 28, 1, 1);		//近端机上行
					display_hz(&chars_1310nm[0], 22, 143, 6);
					PORTG &= ~(1<<PG3);
					PORTG |= (1<<PG4);
					PORTC |= (1<<PC2);               //J5
					PORTF &= ~(1<<PF0);              //J6
					display_hz(&space[0], 13, 86, 14);
					display_img(14, 86, key1[0], 1, 2, 16);
					display_img(16, 86, key1[13], 1, 2, 16);
					if(flagRFOnOff == 1)
					{
						display_img(18, 86, key1[11], 1, 2, 16);
						display_img(20, 86, key1[1], 1, 2, 16);
						display_img(21, 86, key1[0], 1, 2, 16);
					}
					else
					{
						display_hz(&space[0], 18, 86, 6);
					}
					display_img(23, 86, key1[14], 1, 2, 16);
					display_hz(&chars_NPdBm[2], 25, 86, 3);
					
					if(flagPressMhzKey == 1)
					{
						InitADF4118(result1, REF_FRE);
						flagPressMhzKey = 0;
					}
					else
					{
						InitADF4118(UP_GO_DEFAULT, REF_FRE);
					}
				}
				else if(flagRecentFar == 0 && flagUpDown == DOWN_GO_MARK)         //Y-X
				{
                    display_hz(&space[0],12,25,12);
		  			display_img(15,25,E,1,10,16); //射频输出置为E 
		  			putoutcode(92);
					//TX_EN;
					display_hz(&Mode3[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode3[3],28,1,1);         //y-x
					display_hz(&chars_1550nm[0],22,143,6);
					PORTG |= (1<<PG3);
					PORTG &= ~(1<<PG4);
					PORTC &= ~(1<<PC2);                   //J5
					PORTF &= ~(1<<PF0);                   //J6
					display_hz(&space[0],13,86,16);
					display_img(15,86,key1[0],1,2,16);
					display_img(17,86,key1[13],1,2,16);
					if(flagRFOnOff == 1)
					{
						display_img(19,86,key1[0],1,2,16);
					}
					else
					{
						display_hz(&space[0],19,86,2);
					}
					display_img(21,86,key1[14],1,2,16);
					display_hz(&chars_NPdBm[2],23,86,3);
					
					if(flagPressMhzKey == 1)
					{
						InitADF4118(result1, REF_FRE);
						flagPressMhzKey = 0;
					}
					else
					{
						InitADF4118(DOWN_GO_DEFAULT, REF_FRE);
					}
				}
				else if(flagRecentFar == 0 && flagUpDown == UP_GO_MARK)           //Y-S
				{
#if 0				
					if(flagRFOnOff == 1)
					{
						display_img(15,26,key1[11],1,2,16);  		//-92
						display_img(17,26,key1[9],1,2,16);
						display_img(19,26,key1[2],1,2,16);
						display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
						eeprom_write(EEP_DBVALUE,92);
					}	
#endif
		     	    flagRecentFar = 0;
					display_hz(&Mode4[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode4[3],28,1,1);
					display_hz(&chars_1310nm[0],22,143,6);
					PORTG |= (1<<PG3) | (1<<PG4);
                                        
                    //PORTG &= ~(1<<PG3);
					//PORTG |= (1<<PG4);
                                        
					PORTC &= ~(1<<PC2);    //J5
					PORTF |= (1<<PF0);     //J6
					display_hz(&space[0],13,86,16);
					display_img(15,86,E,1,10,16);	
					
					if(flagPressMhzKey == 1)
					{
						InitADF4118(result1, REF_FRE);
						flagPressMhzKey = 0;
					}
					else
					{
						InitADF4118(UP_GO_DEFAULT, REF_FRE);
					}
				}
				refresh_flag = 0;
				refresh1_flag = 1;
			}
			if(flag_ADRF == 1)
			{
				send_list_char((char *)Light_input);
				
				//delay(30000);
				
				if((long int)rev_data[8]/0x10 == 0)      					//收光功率是正值时
				{
					a = ((long int)rev_data[8]*0x100 + rev_data[9]) / 100.0;
					if(rev_data[0] == 0x00)
					{
						display_hz(&space[0],13,118,15);
						display_img(15,118,E,1,10,16); //射频功率 开机时为E
					}
                    else if(a >= 6)
                    {
                      	display_hz(&space[0],13,118,15);
  						display_img(15,118,E,1,10,16); //射频功率 开机时为E
                    }
					else
					{
						a += 0.05;
						b = a * 10;
						display_hz(&space[0],13,118,15);
						display_img(13,118,key1[10],1,2,16);
						temp = b;
						temp1 = temp;
						while(temp1/10!=0)
						{
							 Light_number *= 10;
							 temp1 /= 10;
						}
						if(Light_number == 1)                          		//射频值为0.5形式
						{
							 display_img(15,118,key1[0],1,2,16);
							 display_hz(&dot[0],17,118,1);
							 display_img(18,118,key1[temp],1,2,16);
							 display_hz(&chars_NPdBm[2],21,118,3);
						}                                                     //射频值为1.5形式
						else if(Light_number == 10)
						{
							display_img(15,118,key1[temp/10],1,2,16);
							display_hz(&dot[0],17,118,1);
							display_img(18,118,key1[temp%10],1,2,16);
							display_hz(&chars_NPdBm[2],21,118,3);
							Light_number = 1;
						}                                                     //射频值为12.5形式
						else if(Light_number == 100)
						{
							display_img(15,118,key1[temp/100],1,2,16);
							temp %= 100;
							display_img(17,118,key1[temp/10],1,2,16);
							display_hz(&dot[0],19,118,1);
							display_img(20,118,key1[temp%10],1,2,16);
							display_hz(&chars_NPdBm[2],23,118,3);
							Light_number = 1;
						}
					}
				}
				else                                                                //收光功率为负值时
				{
					display_hz(&space[0],13,118,15);
					a = (0x10000 - ((long int)rev_data[8]*0x100 + rev_data[9])) / -100.0;
					if(a <= -20)                                                       //没有收到光时显示E
					{
                                               // display_hz(&space[0],13,118,15);
						display_img(15,118,E,1,10,16); //射频功率 开机时为E
					}
					else
					{
						a = a- 0.05;
						b = a * 10;
						display_img(13,118,key1[11],1,2,16);
						b = -b;
						temp = b;
						temp1 = temp;
						while(temp1/10!=0)                                   //计算位数
						{
							 Light_number *= 10;
							 temp1 /= 10;
						}
						if(Light_number == 1)                          		//收光功率值为0.5形式
						{
							 display_img(15,118,key1[0],1,2,16);
							 display_hz(&dot[0],17,118,1);
							 display_img(18,118,key1[temp],1,2,16);
							 display_hz(&chars_NPdBm[2],21,118,3);
						}                                                     //收光功率值为1.5形式
						else if(Light_number == 10)
						{
							display_img(15,118,key1[temp/10],1,2,16);
							display_hz(&dot[0],17,118,1);
							display_img(18,118,key1[temp%10],1,2,16);
							display_hz(&chars_NPdBm[2],21,118,3);
							Light_number = 1;
						}                                                     //收光功率值为12.5形式
						else if(Light_number == 100)
						{
							display_img(15,118,key1[temp/100],1,2,16);
							temp %= 100;
							display_img(17,118,key1[temp/10],1,2,16);
							display_hz(&dot[0],19,118,1);
							display_img(20,118,key1[temp%10],1,2,16);
							display_hz(&chars_NPdBm[2],23,118,3);
							Light_number = 1;
						}
					}
				}
				flag_ADRF = 0;
			}		
		}
		else                                                //中继状态
		{
			if(refresh1_flag == 1)    //由非中继变到中继
			{
				display_freq(); 				//开机显示932.000MHz
	
				display_img(15,26,key1[11],1,2,16);  		//-92
				display_img(17,26,key1[9],1,2,16);
				display_img(19,26,key1[2],1,2,16);
				display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
				eeprom_write(EEP_UP_DOWN_FG, UP_GO_MARK);
				eeprom_write(EEP_DBVALUE,92);
				eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
				eeprom_write(EEP_DB_CHANGE_RANGE,1);
                eeprom_write(EEP_CURRENT_CUR_LOCATION,0x01);                  //位置标志
				eeprom_write(EEP_RF_ON_OFF_FG,1);
				putoutcode(92);
				TX_EN;
				InitADF4118(DOWN_GO_DEFAULT, REF_FRE);
				eeprom_write(EEP_RF_FIRST_BIT, RF_DOWN_FIRST_BIT);
				eeprom_write(EEP_UP_DOWN_FG, RF_DOWN_SECOND_BIT);
			    eeprom_write(EEP_RF_THREAD_BIT, RF_DOWN_THREAD_BIT);
			    eeprom_write(EEP_RF_FOURTH_BIT, RF_BIT_DEFAULT);
			    eeprom_write(EEP_RF_FIFTH_BIT, RF_BIT_DEFAULT);
				result1 = DOWN_GO_DEFAULT;
				refresh1_flag = 0;
				refresh2_flag = 1;
				refresh_flag = 1;
				bgon_off = 0;
			}
			PORTG &= ~((1<<PG3) | (1<<PG4));
			PORTC &= ~(1<<PC2);                      //J5
			PORTF &= ~(1<<PF0);                      //J6
			display_hz(&space[0],13,118,11);
			display_img(15,118,E,1,10,16);
			display_hz(&space[0],13,86,16);
			display_img(15,86,E,1,10,16);
			display_hz(&Mode5[0],25,1,1);
			display_img(26,1,key1[15],1,2,16);
			display_hz(&Mode5[3], 28, 1, 1);         //z-j
			display_hz(&space[0], 22, 143, 6);
			flag_ADRF = 0;
			delay(20000);
		}
   keyvalue = 0;	  
   keyvalue = KeyScan();
   
   if(keyvalue <= 9)                                   //if the down key is a digtal key
   {
     flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);
     flagPressKeyTime = eeprom_read(EEP_CURRENT_CUR_LOCATION);       //flag_f标志第几次按按键
     if(flagRFOnOff == 0)
     { 
       KeyUp(); 
       continue;
     }    
     if(flagPressKeyTime > 6)
     {
       KeyUp();        
       continue;
     }
     if(flagPressKeyTime == 1)                                         //putin a new frequency from NO.1 bit
     { 
       if(keyvalue != RF_UP_FIRST_BIT && keyvalue != RF_DOWN_FIRST_BIT) 
       {
         KeyUp();         
         continue;
       }
       WriteCur(flagPressKeyTime, 1);
       display_hz(&key[keyvalue],14,1,1);
       display_hz(&space[0],15,1,9);
	   bitvalue0 = keyvalue;
       KeyUp();     
       continue;  
     }
     else
     {
       if(flagPressKeyTime == 2)
       {
         if((bitvalue0 == RF_DOWN_FIRST_BIT && (keyvalue != RF_DOWN_SECOND_BIT)) || 
		 	(bitvalue0 == RF_UP_FIRST_BIT)  && (keyvalue != RF_UP_SECOND_BIT))  
         {
           KeyUp();            
           continue;
         }
         WriteCur(flagPressKeyTime,1);
         display_hz(&key[keyvalue],15,1,1);
  
         bitvalue1=keyvalue;  
         KeyUp(); 
         KeyUp();
         KeyUp();
         KeyUp();        
         continue;
       }  
       else
       {
         if(flagPressKeyTime == 6) 
         { 
           if((keyvalue!=0)&&(keyvalue!=2)&&(keyvalue!=5)&&(keyvalue!=7)) 
           {
             KeyUp();
             KeyUp();
             KeyUp();
             KeyUp();             
             continue;
           }
           WriteCur(6,2);   
           switch(keyvalue)
           {
             case 0:
              display_hz(&key[0],19,1,1);
              display_hz(&key[0],20,1,1);
              bitvalue4=0;
             break;
             case 2:
              display_hz(&key[2],19,1,1);
              display_hz(&key[5],20,1,1);
              bitvalue4=1;
             break;
            case 5:
              display_hz(&key[5],19,1,1);
              display_hz(&key[0],20,1,1);
              bitvalue4=2;
            break;
            case 7:
             display_hz(&key[7],19,1,1);
             display_hz(&key[5],20,1,1);
             bitvalue4=3;
            break;
			default:
				break;
          }
            KeyUp();
            KeyUp();
            KeyUp();
            KeyUp();           
            continue;
        }
        else
        {
          if(flagPressKeyTime == 3)
          {
            if((bitvalue0 == RF_DOWN_FIRST_BIT && keyvalue > 4) || 
			   (bitvalue0 == RF_UP_FIRST_BIT && keyvalue < 5))
            {
            	KeyUp();            
                continue;
            }
            WriteCur(flagPressKeyTime, 2);
            display_hz(&key[keyvalue],16,1,1);
            display_hz(&dot[0],17,1,1);///.
            bitvalue2=keyvalue;
			if((bitvalue0 == RF_DOWN_FIRST_BIT && keyvalue == 4) || 
			   (bitvalue0 == RF_UP_FIRST_BIT && keyvalue == 9))
			{
    			display_hz(&key[0],18,1,1);
                display_hz(&key[0],19,1,1);
                display_hz(&key[0],20,1,1);
                display_hz(&chars_MHz[0],21,1,3);
                bitvalue3=0; 
                bitvalue4=0;
                goto wee;
			}
			else
			{
                KeyUp();
                KeyUp();
                KeyUp();
                KeyUp();           
                continue;
	        }
          }
          else
          {
            if(flagPressKeyTime==5)
            {
              WriteCur(5,1);
              display_hz(&key[keyvalue],18,1,1);
              bitvalue3=keyvalue;    
              KeyUp();
              KeyUp();
              KeyUp();
              KeyUp();             
              continue;
            }
          }
        }
      }
    }
  }
  else if(keyvalue == KEY_C)                                 //clear key
  { 
	  delay(50000);
	  if(keyvalue == KEY_C)
	  {
	   flagPressKeyTime = eeprom_read(EEP_CURRENT_CUR_LOCATION);
	   if(flagPressKeyTime == 3) 
	   {
		 clearbit(2);WriteCur(3,10);KeyUp();  continue;
	   }
	   else if(flagPressKeyTime == 5) 
	   {
		 clearbit(3);WriteCur(5,20); KeyUp(); continue;
	   }
	   else if(flagPressKeyTime == 6) 
	   {
		 WriteCur(6,10);clearbit(5);KeyUp(); continue;
	   }  
	   else if(flagPressKeyTime == 8) 
	   {
		 WriteCur(8,20);clearbit(6);KeyUp(); continue;
	   }   
	   else if(flagPressKeyTime == 2)
	   { 
		 clearbit(1);
		 bit0 = eeprom_read(EEP_RF_FIRST_BIT);
		 bit1 = eeprom_read(EEP_UP_DOWN_FG);
		 bit2 = eeprom_read(EEP_RF_THREAD_BIT);
		 bit3 = eeprom_read(EEP_RF_FOURTH_BIT);
		 bit4 = eeprom_read(EEP_RF_FIFTH_BIT);
		 display_hz(&key[bit0],14,1,1);
		 display_hz(&key[bit1],15,1,1);
		 display_hz(&key[bit2],16,1,1);
		 display_hz(&dot[0],17,1,1);
		 display_hz(&key[bit3],18,1,1);
		 switch(bit4)
		 {
		   case 0:
				 display_hz(&key[0],19,1,1);
				 display_hz(&key[0],20,1,1);
				 break;
		   case 1:
				  display_hz(&key[2],19,1,1);
				  display_hz(&key[5],20,1,1);
				  break;
		   case 2:
				  display_hz(&key[5],19,1,1);
				  display_hz(&key[0],20,1,1);
				  break;
		   case 3:
				  display_hz(&key[7],19,1,1);
				  display_hz(&key[5],20,1,1);
				  break;
		   default: 
				  break; 
		  }   
		  display_hz(&chars_MHz[0],21,1,3);
		  eeprom_write(EEP_CURRENT_CUR_LOCATION,1);
		  KeyUp(); 
          KeyUp();
		  continue;
	   }
    }
  }
  else if(keyvalue==KEY_MHz)                         //MHz key
  { 
    flagPressKeyTime = eeprom_read(EEP_CURRENT_CUR_LOCATION);
    if(flagPressKeyTime == 3)
    {
      display_hz(&key[0],16,1,1);
      display_hz(&dot[0],17,1,1);
      display_hz(&key[0],18,1,1);
      display_hz(&key[0],19,1,1);
      display_hz(&key[0],20,1,1);
      display_hz(&chars_MHz[0],21,1,3);
      bitvalue2=0;
      bitvalue3=0;
      bitvalue4=0; 
      goto wee;
    } 
    else if(flagPressKeyTime == 5)
    {
      display_hz(&key[0],18,1,1);
      display_hz(&key[0],19,1,1);
      display_hz(&key[0],20,1,1);
      display_hz(&chars_MHz[0],21,1,3);
      bitvalue3=0; 
      bitvalue4=0;
      goto wee;
    }    
    else if(flagPressKeyTime == 6)
    {
      display_hz(&key[0],19,1,1);
      display_hz(&key[0],20,1,1);
      display_hz(&chars_MHz[0],21,1,3);
      bitvalue4=0; 
      goto wee; 
    }
    else if(flagPressKeyTime==8)
    {
      display_hz(&chars_MHz[0],21,1,3);
      goto wee;
    }
    else 
    { 
      KeyUp();      
      continue;
    }
   wee: 
    eeprom_write(EEP_CURRENT_CUR_LOCATION,0x01);
    result = 0;
	no1value = (((unsigned long)bitvalue0) * 100000);
    no2value = (((unsigned long)bitvalue1) * 10000);
    no3value = (((unsigned long)bitvalue2) * 1000);
    no4value = ((unsigned long)bitvalue3) * 100 + ((unsigned long)bitvalue4) * 25;
    result = no1value + no2value + no3value + no4value; 
	eeprom_write(EEP_RF_FIRST_BIT, bitvalue0);
    eeprom_write(EEP_UP_DOWN_FG, bitvalue1);
    eeprom_write(EEP_RF_THREAD_BIT, bitvalue2);
    eeprom_write(EEP_RF_FOURTH_BIT, bitvalue3);
    eeprom_write(EEP_RF_FIFTH_BIT, bitvalue4);
	result1 = result;
	refresh_flag = 1;
    if(flagRFOnOff==1)                
    { 
		InitADF4118(result, REF_FRE);
		if(relayStatus == 0) 
		{
			flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG); 
			if((flagRecentFar == 1 && bitvalue1 == DOWN_GO_MARK) || (flagRecentFar == 0 && bitvalue1 == UP_GO_MARK))
			{
				display_img(15,26,key1[11],1,2,16);  		//-92
				display_img(17,26,key1[9],1,2,16);
				display_img(19,26,key1[2],1,2,16);
				display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
				eeprom_write(EEP_DBVALUE,92);
				putoutcode(92);
				TX_EN;
			}
		}
		
    }
    if(relayStatus == 0)
    {
		flagPressMhzKey = 1;
    }
    KeyUp();    
    continue;          
  }
  else if (keyvalue == KEY_RF)   //on/off key              
  {
		flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);
		if(flagRFOnOff == 1) //射频现在是打开状态，需要关闭
		{  
		  flagPressKeyTime = eeprom_read(EEP_CURRENT_CUR_LOCATION);
		  if(flagPressKeyTime > 1)
		  {
			  eeprom_write(EEP_CURRENT_CUR_LOCATION,1);
		  }
		  eeprom_write(EEP_RF_ON_OFF_FG,0);
		  TX_UNEN;
		  display_hz(&space[0],14,1,10);
		  display_hz(&space[0],12,25,12);
		  display_img(15,25,E,1,10,16); //射频输出置为E 
		  CUT_RF_POWER;
		} 
		else if(flagRFOnOff==0)
		{
		  TX_EN;
		  //flagRFOnOff=1;  
		  bit0 = eeprom_read(EEP_RF_FIRST_BIT);
		  bit1 = eeprom_read(EEP_UP_DOWN_FG);
		  bit2 = eeprom_read(EEP_RF_THREAD_BIT);
		  bit3 = eeprom_read(EEP_RF_FOURTH_BIT);
		  bit4 = eeprom_read(EEP_RF_FIFTH_BIT);
		  tempValue = bit4;	
		  result = 0;
		  no1value = (((unsigned long)bitvalue0) * 100000);
		  no2value = (((unsigned long)bit1) * 10000);
		  no3value = (((unsigned long)bit2) * 1000);
		  no4value = ((unsigned long)bit3) * 100 + ((unsigned long)tempValue) * 25;
		  result = no1value + no2value + no3value + no4value; 	
		  InitADF4118(result, REF_FRE);
		  result1 = result;
		  if(relayStatus == 0)
		  {
	          flagPressMhzKey = 1;
		  }
		  
		  display_hz(&key[bit0],14,1,1);
		  display_hz(&key[bit1],15,1,1);
		  display_hz(&key[bit2],16,1,1);
		  display_hz(&dot[0],17,1,1);
		  display_hz(&key[bit3],18,1,1);
			  
		 switch(bit4)
		 {
		   case 0:
				  display_hz(&key[0],19,1,1);
				  display_hz(&key[0],20,1,1);
				  break;
		   case 1:
				  display_hz(&key[2],19,1,1);
				  display_hz(&key[5],20,1,1);
				  break;
		   case 2:
				  display_hz(&key[5],19,1,1);
				  display_hz(&key[0],20,1,1);
				  break;
		   case 3:
				  display_hz(&key[7],19,1,1);
				  display_hz(&key[5],20,1,1);
				  break;
		   default: 
				  break; 
		 }   
		 display_hz(&chars_MHz[0],21,1,3);  
		 
		 eeprom_write(EEP_RF_ON_OFF_FG,1);
		if(relayStatus == 2 || (flagRecentFar == 1 && flagUpDown == DOWN_GO_MARK) || (flagRecentFar == 0 && flagUpDown == UP_GO_MARK))
		{
			 dbvalue=eeprom_read(EEP_DBVALUE);   //初始值是92
			 putoutcode(dbvalue);     
			 db_flag=eeprom_read(EEP_DB_PLUS_MINUX_FG);
			 display_hz(&space[0],15,25,10);
			 if(db_flag==0)      
			 {
			   display_img(15,26,key1[11],1,2,16);
			 }
			 else if(db_flag==1)    
			 {
			   display_img(15,26,key1[10],1,2,16);
			 }
			 if(dbvalue<10)      
			 {
			   display_img(17,26,key1[dbvalue],1,2,16);
			 }
			 if (dbvalue==0)     
			 {
			   display_hz(&space[0],15,25,2);
			   display_img(17,26,key1[0],1,2,16);
			 }
			 else if(dbvalue>=10)
			 { 
			   no1bit=(dbvalue%10); 
			   no2bit=(dbvalue)/10;
			   display_img(17,26,key1[no2bit],1,2,16);
			   display_img(19,26,key1[no1bit],1,2,16);
			 }   
			 display_hz(&chars_NPdBm[2],22,25,3); 	//dBm
		}
	   }
	   refresh_flag = 1;
       KeyUp(); 
       KeyUp();
       KeyUp();
       KeyUp();  
       continue;
  }  
  else if (keyvalue==KEY_UP)
  {
      key_flag = eeprom_read(EEP_DB_CHANGE_RANGE);
      if(key_flag == 0)
      {
            delay(65535);//延时防抖
            delay(65535);
      }
      else
      {
          delay(65535);
          delay(30000);
      }
	
      if(keyvalue == KEY_UP)
      {
      	flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
        flagUpDown = eeprom_read(EEP_UP_DOWN_FG);  
      	if(relayStatus)
      	{
      		UpKey();
      	}
        else if((flagRecentFar == 1 && flagUpDown == DOWN_GO_MARK) || (flagRecentFar == 0 && flagUpDown == UP_GO_MARK))//J-X或者Y-S还可以相应上键
        {
            UpKey();
        }
        continue;//"Up" Key
      }
  }
  else if (keyvalue==KEY_DOWN)
  {
      key_flag = eeprom_read(EEP_DB_CHANGE_RANGE);
      if(key_flag == 0)
      {
            delay(65535);//延时防抖
            delay(65535);
      }
      else
      {
              delay(65535);
              delay(30000);
      }
    
      if(keyvalue==KEY_DOWN)
      {
      	flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
		flagUpDown = eeprom_read(EEP_UP_DOWN_FG);  
      	if(relayStatus)
      	{
      		DownKey();
      	}
        else if((flagRecentFar == 1 && flagUpDown == DOWN_GO_MARK) || (flagRecentFar == 0 && flagUpDown == UP_GO_MARK))//J-X或者Y-S还可以相应下键
        {
          DownKey();
        }
        continue;//"Down"
      }
  }
  else if (keyvalue == KEY_LEFT)
  { 
    LeftKey(); 
    KeyUp(); 
    KeyUp();   
    continue;//"Left"
  }
  else if (keyvalue == KEY_RIGHT)
  {
    RightKey();
    KeyUp();
    KeyUp();  
    continue;//"Right"
  }
  else if(keyvalue == KEY_DOWNGO) //xia xing  
  { 
    flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);
	flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
	if(flagRFOnOff == 0)   //When the RF is closed, should open it.
	{
		eeprom_write(EEP_RF_ON_OFF_FG,1);
		
		display_hz(&key[RF_DOWN_FIRST_BIT],14,1,1);
		display_hz(&key[RF_DOWN_SECOND_BIT],15,1,1);
		display_hz(&key[RF_DOWN_THREAD_BIT],16,1,1);
		display_hz(&dot[0],17,1,1);
		display_hz(&key[RF_BIT_DEFAULT],18,1,1);
		display_hz(&key[RF_BIT_DEFAULT],19,1,1);
		display_hz(&key[RF_BIT_DEFAULT],20,1,1);
		display_hz(&chars_MHz[0],21,1,3);
		eeprom_write(EEP_RF_FIRST_BIT, 9);
		eeprom_write(EEP_UP_DOWN_FG, 3);
		eeprom_write(EEP_RF_THREAD_BIT, 2);
		eeprom_write(EEP_RF_FOURTH_BIT, 0);
		eeprom_write(EEP_RF_FIFTH_BIT, 0);
		eeprom_write(EEP_CURRENT_CUR_LOCATION,0x01);
		result = UP_GO_DEFAULT;
		InitADF4118(result, REF_FRE);	  
		flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);                       //近端机or远端机标志位
		flagUpDown = eeprom_read(EEP_UP_DOWN_FG);                   //上行or下行标志位
		
		if(relayStatus == 2 || (flagRecentFar == 1 && flagUpDown == DOWN_GO_MARK) || (flagRecentFar == 0 && flagUpDown == UP_GO_MARK))//J-X或者Y-S才可以显示-92dBm
		{
			display_img(15,26,key1[11],1,2,16);  		//-92
			display_img(17,26,key1[9],1,2,16);
			display_img(19,26,key1[2],1,2,16);
			display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
		    eeprom_write(EEP_DBVALUE,92);
			putoutcode(92);
			TX_EN;
		}
		
		//eeprom_write(EEP_DBVALUE,92);
		eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
		eeprom_write(EEP_DB_CHANGE_RANGE, 1);
		eeprom_write(EEP_RF_ON_OFF_FG, 1);
		//putoutcode(92);
		//TX_EN;
		//refresh_flag = 1;
		//continue;
	}
    if(flagRFOnOff == 1) 
    {
      display_hz(&key[RF_DOWN_FIRST_BIT],14,1,1);
      display_hz(&key[RF_DOWN_SECOND_BIT],15,1,1);
      display_hz(&key[RF_DOWN_THREAD_BIT],16,1,1);
      display_hz(&dot[0],17,1,1);
      display_hz(&key[RF_BIT_DEFAULT],18,1,1);
      display_hz(&key[RF_BIT_DEFAULT],19,1,1);
      display_hz(&key[RF_BIT_DEFAULT],20,1,1);
      display_hz(&chars_MHz[0],21,1,3);
	  eeprom_write(EEP_RF_FIRST_BIT, RF_DOWN_FIRST_BIT);
      eeprom_write(EEP_UP_DOWN_FG, RF_DOWN_SECOND_BIT);
      eeprom_write(EEP_RF_THREAD_BIT, RF_DOWN_THREAD_BIT);
      eeprom_write(EEP_RF_FOURTH_BIT, RF_BIT_DEFAULT);
      eeprom_write(EEP_RF_FIFTH_BIT, RF_BIT_DEFAULT);
      eeprom_write(EEP_CURRENT_CUR_LOCATION,0x01);
      result = DOWN_GO_DEFAULT;
      InitADF4118(result, REF_FRE);	  
    }
	if(relayStatus == 0)                                 //射频和光都工作时
	{
		if(flagRecentFar == 1)                             //近端机
		{
			display_img(15,26,key1[11],1,2,16);  		//-92
			display_img(17,26,key1[9],1,2,16);
			display_img(19,26,key1[2],1,2,16);
			display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
			eeprom_write(EEP_DBVALUE,92);
            putoutcode(92);
			TX_EN;                   
			 display_hz(&Mode1[0],25,1,1);
			 display_img(26,1,key1[15],1,2,16);
			 display_hz(&Mode1[3],28,1,1);       //j-x
			 display_hz(&chars_1550nm[0],22,143,6);
			 PORTG &= ~((1<<PG3) | (1<<PG4));
			 PORTC &= ~(1<<PC2);    //J5
			 PORTF |= (1<<PF0);     //J6
			 display_hz(&space[0],13,86,16);
			 display_img(15,86,E,1,10,16);//发光输出 开机为E
		}
		else                                        //远端机  y-x
		{
			display_hz(&space[0],12,25,12);
			display_img(15,25,E,1,10,16); //射频输出置为E 
            putoutcode(92);
			TX_EN;
			flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);            
			display_hz(&Mode3[0],25,1,1);
			display_img(26,1,key1[15],1,2,16);
			display_hz(&Mode3[3],28,1,1);         
			display_hz(&chars_1550nm[0],22,143,6);
			PORTG |= (1<<PG3);
			PORTG &= ~(1<<PG4);
			PORTC &= ~(1<<PC2);
			PORTF &= ~(1<<PF0);
			display_hz(&space[0],13,86,16);
			display_img(15,86,key1[0],1,2,16);
			display_img(17,86,key1[13],1,2,16);
			if(flagRFOnOff == 1)
			{
				display_img(19,86,key1[0],1,2,16);
			}
			display_img(21, 86, key1[14], 1, 2, 16);
			display_hz(&chars_NPdBm[2], 23, 86, 3);
		}
		eeprom_write(EEP_UP_DOWN_FG, DOWN_GO_MARK);
	}
    KeyUp();    
    continue;           
  }
  else if(keyvalue==KEY_UPGO)  //shang xing
  { 
    flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);
	flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
	if(flagRFOnOff == 0)   //When the RF is closed, should open it.
	{
		eeprom_write(EEP_RF_ON_OFF_FG,1);
		display_hz(&key[RF_UP_FIRST_BIT], 14, 1, 1);
		display_hz(&key[RF_UP_SECOND_BIT], 15, 1, 1);
		display_hz(&key[RF_UP_THREAD_BIT], 16, 1, 1);
		display_hz(&dot[0], 17, 1, 1);
		display_hz(&key[RF_BIT_DEFAULT], 18, 1, 1);
		display_hz(&key[RF_BIT_DEFAULT], 19, 1, 1);
		display_hz(&key[RF_BIT_DEFAULT], 20, 1, 1);
		display_hz(&chars_MHz[0],21,1,3);

        eeprom_write(EEP_RF_FIRST_BIT, 8);
		eeprom_write(EEP_UP_DOWN_FG, 8);
		eeprom_write(EEP_RF_THREAD_BIT, 7);
		eeprom_write(EEP_RF_FOURTH_BIT, 0);
		eeprom_write(EEP_RF_FIFTH_BIT, 0);

		eeprom_write(EEP_CURRENT_CUR_LOCATION, 0x01);
		result = UP_GO_DEFAULT;
		InitADF4118(result, REF_FRE);
		//TX_UNEN; 
		if(relayStatus == 2 || (flagRecentFar == 1 && flagUpDown == DOWN_GO_MARK) || (flagRecentFar == 0 && flagUpDown == UP_GO_MARK))//J-X或者Y-S才可以显示-92dBm
		{
			display_img(15,26,key1[11],1,2,16);  		//-92
			display_img(17,26,key1[9],1,2,16);
			display_img(19,26,key1[2],1,2,16);
			display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm			
			eeprom_write(EEP_DBVALUE,92);
			putoutcode(92);
			TX_EN;
		}
		
		//eeprom_write(EEP_DBVALUE,92);
		eeprom_write(EEP_DB_PLUS_MINUX_FG,0);
		eeprom_write(EEP_DB_CHANGE_RANGE,1);
		eeprom_write(EEP_RF_ON_OFF_FG,1);
		//putoutcode(92);
		//TX_EN;
		refresh_flag = 1;
		continue;
	}

    if(flagRFOnOff==1) 
    {	
      display_hz(&key[RF_UP_FIRST_BIT],14,1,1);
      display_hz(&key[RF_UP_SECOND_BIT],15,1,1);
      display_hz(&key[RF_UP_THREAD_BIT],16,1,1);
      display_hz(&dot[0], 17, 1, 1);
      display_hz(&key[RF_BIT_DEFAULT], 18, 1, 1);
      display_hz(&key[RF_BIT_DEFAULT], 19, 1, 1);
      display_hz(&key[RF_BIT_DEFAULT], 20, 1, 1);
      display_hz(&chars_MHz[0], 21, 1, 3);

	  eeprom_write(EEP_RF_FIRST_BIT, RF_UP_FIRST_BIT);
	  eeprom_write(EEP_UP_DOWN_FG, RF_UP_SECOND_BIT);
      eeprom_write(EEP_RF_THREAD_BIT, RF_UP_THREAD_BIT);
      eeprom_write(EEP_RF_FOURTH_BIT, RF_BIT_DEFAULT);
      eeprom_write(EEP_RF_FIFTH_BIT, RF_BIT_DEFAULT);
    
      eeprom_write(EEP_CURRENT_CUR_LOCATION, 0x01);
      result = UP_GO_DEFAULT;
      InitADF4118(result, REF_FRE);
    }
	
	if(relayStatus == 0)                               //射频和光都工作
	{
		if(flagRecentFar==1)
		{
			display_hz(&space[0],12,25,12);
			display_img(15,25,E,1,10,16); //射频输出置为E 
            putoutcode(92);
			TX_EN;      
			display_hz(&Mode2[0],25,1,1);
			display_img(26,1,key1[15],1,2,16);
			display_hz(&Mode2[3],28,1,1);		//近端机上行
			display_hz(&chars_1310nm[0],22,143,6);
			PORTG &= ~(1<<PG3);
			PORTG |= (1<<PG4);
			PORTC |= (1<<PC2);
			PORTF &= ~(1<<PF0);
			display_hz(&space[0],13,86,14);
			display_img(14,86,key1[0],1,2,16);
			display_img(16,86,key1[13],1,2,16);
			if(flagRFOnOff == 1)
			{
				display_img(18,86,key1[11],1,2,16);
				display_img(20,86,key1[1],1,2,16);
				display_img(21, 86, key1[0], 1, 2, 16);
			}
			display_img(23,86,key1[14],1,2,16);
			display_hz(&chars_NPdBm[2],25,86,3);
		}
		else												//Y-S
		{
			display_img(15,26,key1[11],1,2,16);  		//-92
			display_img(17,26,key1[9],1,2,16);
			display_img(19,26,key1[2],1,2,16);
			display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
			eeprom_write(EEP_DBVALUE,92);
			putoutcode(92);
			TX_EN;
                                
			display_hz(&Mode4[0],25,1,1);     
			display_img(26,1,key1[15],1,2,16);
			display_hz(&Mode4[3],28,1,1); 
			display_hz(&chars_1310nm[0],22,143,6);
			PORTG |= (1<<PG3) | (1<<PG4);
			PORTC &= ~(1<<PC2);    //J5
			PORTF |= (1<<PF0);     //J6
			display_hz(&space[0],13,86,16);
			display_img(15,86,E,1,10,16);
		}
		eeprom_write(EEP_UP_DOWN_FG, UP_GO_MARK);
	}
    KeyUp();     
    continue;           
  }
  else if(keyvalue==KEY_BEIGUANG)     //bei guang
  {
    portd=PORTD&0x80;
    if(portd==0x80)
    {
		EL_CLOSE;
    }
    else if(portd==0x00)
   	{ 
     	EL_EN;
   	} 
    KeyUp();     
    continue;  
  }
  else if(keyvalue==KEY_dBm_W)
  {
	  
	  dw_flag = eeprom_read(EEP_UNIT_FG);               //单位标志位
	
	  if(dw_flag == 1)       //现在的单位是dbm 要变成W
	  {
		  eeprom_write(EEP_UNIT_FG,0);
	  }
	  else
	  {
		  eeprom_write(EEP_UNIT_FG,1);
	  }
	  KeyUp();   
	  continue;
   }
	else if(keyvalue == KEY_RECENT)
	{
		delay(65535);//延时防抖
	  	if(keyvalue==KEY_RECENT)
		{
			if(relayStatus == 0)  //光和射频都工作
			{
				//flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
				flagUpDown = eeprom_read(EEP_UP_DOWN_FG);
				flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);
				if(flagRFOnOff == 0)
				{
					display_freq(); 				//开机显示467.7MHz
					display_img(15,26,key1[11],1,2,16);  		//-92
					display_img(17,26,key1[9],1,2,16);
					display_img(19,26,key1[2],1,2,16);
					display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
					
					eeprom_write(EEP_UP_DOWN_FG,DOWN_GO_MARK);
					eeprom_write(EEP_DBVALUE,92);
					eeprom_write(EEP_DB_PLUS_MINUX_FG,0);
					eeprom_write(EEP_DB_CHANGE_RANGE,1);
					eeprom_write(EEP_RF_ON_OFF_FG,1);
					putoutcode(92);
					TX_EN;
					InitADF4118(DOWN_GO_DEFAULT, REF_FRE);
					refresh_flag = 1;
				}
				else if(flagUpDown == DOWN_GO_MARK)             		//J-X
				{
					display_img(15,26,key1[11],1,2,16);  		//-92
					display_img(17,26,key1[9],1,2,16);
					display_img(19,26,key1[2],1,2,16);
					display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
                    eeprom_write(EEP_DBVALUE,92);
					putoutcode(92);
					TX_EN;
					display_hz(&Mode1[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode1[3],28,1,1);
					display_hz(&chars_1550nm[0],22,143,6);
					PORTG &= ~((1<<PG3) | (1<<PG4));
					PORTC &= ~(1<<PC2);    //J5
					PORTF |= (1<<PF0);     //J6
					display_hz(&space[0],13,86,16);
					display_img(15,86,E,1,10,16);
				}
				else  if(flagUpDown == UP_GO_MARK)               //J-S
				{
					display_hz(&space[0],12,25,12);
					display_img(15,25,E,1,10,16); //射频输出置为E 
                  	putoutcode(92);
					TX_EN;
					display_hz(&Mode2[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode2[3],28,1,1);
					display_hz(&chars_1310nm[0],22,143,6);
					PORTG &= ~(1<<PG3);
					PORTG |= (1<<PG4);
					PORTC |= (1<<PC2);
					PORTF &= ~(1<<PF0);
					display_hz(&space[0],13,86,14);
					display_img(14,86,key1[0],1,2,16);
					display_img(16,86,key1[13],1,2,16);
					if(flagRFOnOff == 1)
					{
						display_img(18,86,key1[11],1,2,16);
						display_img(20,86,key1[1],1,2,16);
						display_img(21,86,key1[0],1,2,16);
					}
					display_img(23,86,key1[14],1,2,16);
					display_hz(&chars_NPdBm[2],25,86,3);
				}
				else
				{
					;
				}
				eeprom_write(EEP_RECENT_FAR_FG,1);
			}
		}
		//KeyUp();   
	  	continue;
	}
	else if(keyvalue == KEY_FAR)
	{
		delay(65535);//延时防抖
	  	if(keyvalue == KEY_FAR)
		{
			if(relayStatus == 0)
			{
				//flagRecentFar = eeprom_read(EEP_RECENT_FAR_FG);
				flagUpDown = eeprom_read(EEP_UP_DOWN_FG);
				flagRFOnOff = eeprom_read(EEP_RF_ON_OFF_FG);
				if(flagRFOnOff == 0)
				{
					display1_freq(); 				//显示887.000MHz
		
					display_img(15,26,key1[11],1,2,16);  		//-92
					display_img(17,26,key1[9],1,2,16);
					display_img(19,26,key1[2],1,2,16);
					display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
                                        
					eeprom_write(EEP_UP_DOWN_FG, UP_GO_MARK);
					eeprom_write(EEP_DBVALUE, 92);
                    eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
				    eeprom_write(EEP_DB_CHANGE_RANGE, 1);
					eeprom_write(EEP_RF_ON_OFF_FG, 1);
					putoutcode(92);
					TX_EN;
					InitADF4118(UP_GO_DEFAULT, REF_FRE);
					refresh_flag = 1;
				}
				
				else if(flagUpDown == DOWN_GO_MARK)                      //Y-X
				{
					display_hz(&space[0],12,25,12);
					display_img(15,25,E,1,10,16); //射频输出置为E 
                    putoutcode(92);
					TX_EN;    
					display_hz(&Mode3[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode3[3],28,1,1);
					display_hz(&chars_1550nm[0],22,143,6);
					PORTG |= (1<<PG3);
					PORTG &= ~(1<<PG4);
					PORTC &= ~(1<<PC2);
					PORTF &= ~(1<<PF0);
					display_hz(&space[0],13,86,16);
					display_img(15,86,key1[0],1,2,14);
					display_img(17,86,key1[13],1,2,16);
					if(flagRFOnOff == 1)
					{
						display_img(19,86,key1[0],1,2,16);
					}
					display_img(21,86,key1[14],1,2,16);
					display_hz(&chars_NPdBm[2],23,86,3);
				}
				else if(flagUpDown == UP_GO_MARK)                //Y-S
				{
					display_img(15,26,key1[11],1,2,16);  		//-92
					display_img(17,26,key1[9],1,2,16);
					display_img(19,26,key1[2],1,2,16);
					display_hz(&chars_NPdBm[2],22,26,3);        //单位dBm
					eeprom_write(EEP_DBVALUE,92);
					putoutcode(92);
					TX_EN;
                                
					flagRecentFar = 0;
					display_hz(&Mode4[0],25,1,1);
					display_img(26,1,key1[15],1,2,16);
					display_hz(&Mode4[3],28,1,1);
					display_hz(&chars_1310nm[0],22,143,6);
					PORTG |= (1<<PG3) | (1<<PG4);
					PORTC &= ~(1<<PC2);    //J5
					PORTF |= (1<<PF0);     //J6
					display_hz(&space[0],13,86,16);
					display_img(15,86,E,1,10,16);
				}
				eeprom_write(EEP_RECENT_FAR_FG,0);
			}
		}
		//KeyUp();   
	  	continue;
	}
	
 }
  
} 
void LeftKey(void)
{ 
	uchar flagRFOnOff;
	flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);
	if(flagRFOnOff==1)
	{
		eeprom_write(EEP_DB_CHANGE_RANGE,0);
	} 
}     

void RightKey(void)
{ 
	uchar flagRFOnOff; 
	flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);
	if(flagRFOnOff==1)
	{
		eeprom_write(EEP_DB_CHANGE_RANGE,1);
	}
} 

void DownKey(void)
{ 
  key_flag=eeprom_read(EEP_DB_CHANGE_RANGE);                   //加减10（=0）还是加减1
  dbvalue=eeprom_read(EEP_DBVALUE);                    
  db_flag=eeprom_read(EEP_DB_PLUS_MINUX_FG);                    //正负标志  0为负 1为正
  flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);                    //射频on/off标志  1是on

  if(flagRFOnOff==1)                  //射频on/off标志
  {
    if((db_flag==0)&&(dbvalue!=0))          //db_flag为0标示现在的值为负
    { 
      no1bit=(dbvalue%10); 
      no2bit=(dbvalue)/10; 
      if(key_flag==0)                   //加减10
      { 
        if(no2bit<8)                  //当值为82以上时不能再减10，否则会越界
        { 
          no2bit=no2bit+1;
          dbvalue=dbvalue+10;        
          display_img(17,26,key1[no2bit],1,2,16);
          display_img(19,26,key1[no1bit],1,2,16);
        }
        else if(no2bit==8)  
        {
          if(no1bit<3)
          {
            no2bit=no2bit+1;
            dbvalue=dbvalue+10;    
            display_img(17,26,key1[no2bit],1,2,16);
            display_img(19,26,key1[no1bit],1,2,16);
          }
        }       
      }
      else if(key_flag==1)
      { 
        if(dbvalue==92)  ;
        else if((no1bit==0)&&(no2bit!=0))
        {   
          display_img(17,26,key1[no2bit],1,2,16);
          display_img(19,26,key1[1],1,2,16);
          dbvalue=dbvalue+1;
        }
        else if(((no1bit!=0)&&(no1bit!=9))&&(no2bit!=0))
        { 
          no1bit=no1bit+1; 
          display_img(17,26,key1[no2bit],1,2,16);
          display_img(19,26,key1[no1bit],1,2,16);
          dbvalue=dbvalue+1;
        }
        else if(((no1bit!=0)&&(no1bit!=9))&&(no2bit==0))
        { 
          no1bit=no1bit+1;;
          display_img(17,26,key1[no1bit],1,2,16);
          display_hz(&space[0],19,25,2);
          dbvalue=dbvalue+1;
        }
        else if(no1bit==9)
        { 
          no2bit=no2bit+1;   
          display_img(17,26,key1[no2bit],1,2,16);
          display_img(19,26,key1[0],1,2,16);
          dbvalue=dbvalue+1; 
        }
      }
    }
    else if((db_flag==0)&&(dbvalue==0))
    { 
      if(key_flag==0)
      { 
        display_img(15,26,key1[11],1,2,16);     //-
        display_img(17,26,key1[1],1,2,16);
        display_img(19,26,key1[0],1,2,16);
        dbvalue=10;
      }
      else if(key_flag==1)
      {
        display_img(15,26,key1[11],1,2,16);     //-
        display_img(17,26,key1[1],1,2,16);
        display_hz(&space[0],19,25,2);
        dbvalue=1;        
      }  
    }
    else if(db_flag==1)
    { 
      if(key_flag==0) 
      { 
        if(dbvalue!=0)
        {
          dbvalue=10-dbvalue;       
          display_img(15,26,key1[11],1,2,16);     //-
          display_img(17,26,key1[dbvalue],1,2,16);
          display_hz(&space[0],19,25,2);
        }
        else if(dbvalue==0)
        {       
          display_img(15,26,key1[11],1,2,16);     //-
          display_img(17,26,key1[1],1,2,16);
          display_img(19,26,key1[0],1,2,16);
          dbvalue=10;  
        } 
        eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
      }
      else if(key_flag==1)
      { 
        if(dbvalue>1)
        {
          dbvalue=dbvalue-1;          
          display_img(17,26,key1[dbvalue],1,2,16);
          display_hz(&space[0],19,25,2);
        }
        else if(dbvalue==1)
        {     
          display_hz(&space[0],15,25,2);
          display_img(17,26,key1[0],1,2,16);
          display_hz(&space[0],19,25,2);
          dbvalue=0; 
        }
        else if(dbvalue==0)
        {     
          display_img(15,26,key1[11],1,2,16);     //-
          display_img(17,26,key1[1],1,2,16);
          display_hz(&space[0],19,25,2);
          dbvalue=1; 
          eeprom_write(EEP_DB_PLUS_MINUX_FG, 0);
        } 
      }  
    }  
    eeprom_write(EEP_DBVALUE,dbvalue);
    putoutcode(dbvalue); 
 }         
} 

void UpKey(void)
{ 
  key_flag=eeprom_read(EEP_DB_CHANGE_RANGE);
  dbvalue=eeprom_read(EEP_DBVALUE);
  db_flag=eeprom_read(EEP_DB_PLUS_MINUX_FG);
  flagRFOnOff=eeprom_read(EEP_RF_ON_OFF_FG);
  if(flagRFOnOff==1)  
  {  
    if((db_flag==0)&&(dbvalue!=0))    //现在的值为负数
    { 
      no1bit=(dbvalue%10);          //个位
      no2bit=(dbvalue/10);   //十位
      if(key_flag==0)               //现在的值为加10模式
      { 
        if((no2bit==0)&&(no1bit>6))  //当值在-1到-6这个区间时是不能加10的 否则会过界       
        { 
          no1bit=10-no1bit;
          dbvalue=no1bit;
          if(dbvalue==0)
          {        
             display_hz(&space[0],15,25,1);
             display_img(17,26,key1[0],1,2,16);
          }
          else
          {   
            display_img(15,26,key1[10],1,2,16);
            display_img(17,26,key1[no1bit],1,2,16);
            display_hz(&space[0],19,25,2);
          }  
          eeprom_write(EEP_DB_PLUS_MINUX_FG, 1);           //变为正数      db_flag
        }
        else if(no2bit!=0)                //十位不为0时
        {
          no2bit=no2bit-1;                 //十位减1
          dbvalue=dbvalue-10;              //总的值减10
          if(dbvalue>=10) 
          {
           display_img(17,26,key1[no2bit],1,2,16);
          }
          else if((dbvalue>0)&&(dbvalue<10)) 
          {     
            display_img(17,26,key1[no1bit],1,2,16);
            display_hz(&space[0],19,25,2);
          } 
          else if(dbvalue==0)
          {  
           display_hz(&space[0],15,25,2);
           display_img(17,26,key1[0],1,2,16);
           display_hz(&space[0],19,25,2);
          } 
        } 
      }
      else if(key_flag==1)                  //加减1
      { 
        if((no1bit==0)&&(no2bit==1)) 
        { 
          display_hz(&space[0],19,25,2);
          display_img(17,26,key1[9],1,2,16);
          dbvalue=dbvalue-1; 
        } 
        else if((no1bit==0)&&(no2bit>1))
        { 
          no2bit=no2bit-1;
          display_img(19,26,key1[9],1,2,16);
          display_img(17,26,key1[no2bit],1,2,16);
          dbvalue=dbvalue-1;
        }
        else if((no1bit!=0)&&(no2bit!=0))
        { 
          no1bit=no1bit-1; 
          display_img(17,26,key1[no2bit],1,2,16);
          display_img(19,26,key1[no1bit],1,2,16);
          dbvalue=dbvalue-1;
        }
        else if ((no1bit>1)&&(no2bit==0))
        {          
          no1bit=no1bit-1;
          display_img(17,26,key1[no1bit],1,2,16);
          display_hz(&space[0],19,25,2);
          dbvalue=dbvalue-1;
        }
        else if((no1bit==1)&&(no2bit==0)) 
        {         
          display_hz(&space[0],15,25,2);
          display_img(17,26,key1[0],1,2,16);
          display_hz(&space[0],19,25,2);
          dbvalue=0;
        } 
      }
    }
    else if((db_flag==0)&&(dbvalue==0))           //当值为0时
    { 
      if(key_flag==0)   ;                         //如果当前模式是加减10 则不作处理
      else if(key_flag==1)                        //当前模式为加减1
      {
        display_img(15,26,key1[10],1,2,16);
        display_img(17,26,key1[1],1,2,16);
        display_hz(&space[0],19,25,2);
        dbvalue=1;
        eeprom_write(EEP_DB_PLUS_MINUX_FG, 1); 
      }  
    }
    else if(db_flag==1)                             //当前值为正
    { 
      if(key_flag==0);                              //加减10模式
      else if(key_flag==1)                          //加减1
      {
        dbvalue=dbvalue+1;
        if (dbvalue>3) 
          dbvalue=3;
        else 
        { 
           display_img(15,26,key1[10],1,2,16);              //+号
           display_img(17,26,key1[dbvalue],1,2,16);
           display_hz(&space[0],19,25,2);
        } 
      }  
    }
    eeprom_write(EEP_DBVALUE,dbvalue);
    putoutcode(dbvalue);  
  }   
}    


void putoutcode(uchar dbvalue)
{ 
	db_flag=eeprom_read(EEP_DB_PLUS_MINUX_FG);
	if(db_flag==0)                                            //标志现在的值为负
	{ 
		dbvalue=dbvalue+3;                                       //加6什么意思
		if(dbvalue<32)
		{ 
	  		PORTD=PORTD&0x80|0x60|dbvalue;
		}
		else if(dbvalue==32)
		{
	  		PORTD=PORTD&0xa0|0x20;
		}                          
		else if((dbvalue>32)&&(dbvalue<64))
		{
			dbvalue=dbvalue-32;
			PORTD=PORTD&0x80|0x20|dbvalue;
		}                             
		else
		{ 
			dbvalue=dbvalue-64;
			DDRD=DDRD|0x7f;
			PORTD=PORTD&0x80|dbvalue;
		}
	}
	else
	{ 
		dbvalue=3-dbvalue;
		DDRD=DDRD&0x80|0x7f;
		PORTD=PORTD&0x80|dbvalue|0x60; 
	} 
}  
