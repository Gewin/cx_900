#include "iom128.h"
#include "ina90.h"
#include"math.h"                                      //164#

#include "cx_common.h"
#include"LCD_240160.h"

volatile long int flagtime = 0,bgon_off = 0,beiguang_off = 0;

volatile uint flag_ADRF = 1;
volatile uchar flagUpDown;
volatile uchar refresh_w_flag = 0;
volatile float value=0;
volatile int fengming_num = 0;
volatile uchar dw_flag;                       //单位标志位 1表示dbm 0表示w
volatile float RF_value,dbvalue_w;				//变换射频功率单位时用到的变量

uchar no1_bit,no2_bit,no3_bit,portd;

float RF_acc,RFacc_temp,Battery_acc;    //RFacc_temp是一个中间变量，如果没有这个中间变量 那么RF_acc会累加，显示结果就不正确了。
volatile uint flag_battery = 1;                //控制让电池报警直响一次

void Battery_Dis()
{
	uchar m,n,j,i;
	if(Battery_acc>=2850)
	{
		fengming_num = 0;
		display_img(12,148,battery[3],1,6,8);//电池电量
	}
	else if(Battery_acc>2600&&Battery_acc<2850)
	{
		fengming_num = 0;
		if(Battery_acc>2600&&Battery_acc<2845)
			display_img(12,148,battery[2],1,6,8);//电池电量
	}		
	else if(Battery_acc>=2260&&Battery_acc<=2600)
	{
		fengming_num = 0;
		if(Battery_acc>=2260&&Battery_acc<=2585)
			display_img(12,148,battery[1],1,6,8);//电池电量
	}	
	else if(Battery_acc<2260)
	{
		if(Battery_acc<2250)
		{
			display_img(12,148,battery[0],1,6,8);//电池电量
			
			if(fengming_num<3)
			{
				if(beiguang_off % 2 == 0)
				{
					for(m=0;m<2;m++)         //feng ming xiang 
					{
						for(j=0;j<3;j++)
						{
							for(n=0;n<0xff;n++)
							{ 
								DDRB=DDRB&0xdf|0x20;     
								PORTB=PORTB&0xdf|0x20;  
								for(i=0;i<0x60;i++)  ;  
								DDRB=DDRB&0xdf|0x20;     
								PORTB=PORTB&0xdf; 
								for(i=0;i<0x60;i++)  ; 
							}
						} 
						for(n=0;n<0xff;n++)    
						{ 
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf;
							for(i=0;i<0xff;i++)  ;
							for(i=0;i<0xff;i++)  ;
							for(i=0;i<0xff;i++)  ;
							for(i=0;i<0xff;i++)  ;
							for(i=0;i<0xff;i++)  ;
							//for(i=0;i<0xff;i++)  ;
							//for(i=0;i<0xff;i++)  ;
						}
					}  
					//beiguang_off++; 
					fengming_num++;
					//flagtime = 0;
				}	
				flagtime = 0;
			} 
			if(flagtime >= 30)
			{
				PORTB |= (1<<PB7);
			}
		}
	}
}

void RF_Dis()
{
	int value_temp;
	uchar m,n,j,i;
	RFacc_temp = RF_acc;
	flagUpDown = eeprom_read(EEP_UP_DOWN_FG);					//这一位是5表示是上行，是6表示是下行
	if(flagUpDown == UP_GO_MARK)                              //Shang xing
	{
		portd=PORTD;
		portd=portd&0x1F;    
		switch(portd)
		{ 
		case 0x00:               //+3           //2475
			RFacc_temp=RFacc_temp;
			break;
		case 0x01:          
			RFacc_temp=RFacc_temp-18;         //2440
			break;
		case 0x02: 
			RFacc_temp=RFacc_temp-27;           //2395
			break; 
		case 0x03:               //0
			RFacc_temp=RFacc_temp-36;           //2350
			break;
		case 0x04:                                  //-1
			RFacc_temp=RFacc_temp-18;           //2305
			break; 
		case 0x05:                                  //-2
			RFacc_temp=RFacc_temp-27;           //2260
			break;
		case 0x06:             //-3
			RFacc_temp=RFacc_temp-27;          //2215mv
			break; 
		case 0x07:              //-4
			RFacc_temp=RFacc_temp-36;         //2170mv
			break;
		case 0x08: 
			RFacc_temp=RFacc_temp-18;          //-5    2125
			break; 
		case 0x09:                  
			RFacc_temp=RFacc_temp-27;        //-6  2080
			break; 
		case 0x0A: 
			RFacc_temp=RFacc_temp-18;          //-7    2035
			break;
		case 0x0B: 
			RFacc_temp=RFacc_temp-36;           //-8   1990
			break; 
		case 0x0C:             
			RFacc_temp=RFacc_temp-9;     //-9     1945
			break;
		case 0x0D: 
			RFacc_temp=RFacc_temp-13.5;      //-10   1900
			break; 
		case 0x0E: 
			RFacc_temp=RFacc_temp-18;      //-11   1855
			break;
		case 0x0F: 
			RFacc_temp=RFacc_temp-18;        //-12   1810
			break; 
		case 0x10:            
			RFacc_temp=RFacc_temp;      //-13   1765
			break;
		case 0x11:             
			RFacc_temp=RFacc_temp-18;        //-14   1720
			break; 
		case 0x12:             
			RFacc_temp=RFacc_temp-9;      //-15   1675
			break;
		case 0x13: 
			RFacc_temp=RFacc_temp-27;      //-16   1630
			break; 
		case 0x14:         
			RFacc_temp=RFacc_temp;        //-17   1585
			break;
		case 0x15: 
			RFacc_temp=RFacc_temp-9;        //-18   1540
			break; 
		case 0x16: 
			RFacc_temp=RFacc_temp-9;      //-19   1495
			break;
		case 0x17:         
			RFacc_temp=RFacc_temp-18;       //-20    1450
			break; 
		case 0x18: 
			RFacc_temp=RFacc_temp-9;       //-21    1405
			break;
		case 0x19: 
			RFacc_temp=RFacc_temp-13.5;     //-22    1360
			break; 
		case 0x1A: 
			RFacc_temp=RFacc_temp-13.5;       //-23    1315
			break;  
		case 0x1B:       
			RFacc_temp=RFacc_temp-27;       //-24    1270
			break;
		case 0x1C: 
			RFacc_temp=RFacc_temp;    //-25   1225
			break; 
		case 0x1D: 
			RFacc_temp=RFacc_temp-9;      //-26   1180
			break;
		case 0x1E: 
			RFacc_temp=RFacc_temp-9;      //-27   1135mv
			break; 
		case 0x1F:                //-28
			RFacc_temp=RFacc_temp-18;             //1090mv
			break;
		
		default:
			break; 
	 }  
         portd=PORTD;
		portd=portd&0x7F; 
                switch(portd)
                {
                    case 0x20:
			RFacc_temp = RFacc_temp;          //-29
			break;
		    case 0x21:
			RFacc_temp = RFacc_temp+9;          //-30
			break;
                    case 0x22:
                        RFacc_temp = RFacc_temp;         //-31
                        break;
                    default:
                        break;
                }
	 
		if(RFacc_temp<990)/////////////////////display wrong   
		 {
			display_hz(&space[0],13,55,14);
			display_img(15,55,E,1,10,16); 
		 } 
	 
		else if((RFacc_temp>=990)&&(RFacc_temp<1590))      //-30dBm~-15dBm   adjust 49mv/db    -30dBm__1000mv  
		{
		   no1_bit=((int)(RFacc_temp-990))%40;
		   no2_bit=(int)(RFacc_temp-990-no1_bit)/40;   
		   no3_bit=(int)(no1_bit/4.0);
		   
		   value=30-no2_bit-(float)(0.1*no3_bit);
		   
		   if(value>30)
		   {
				display_hz(&space[0],13,55,14);
				display_img(15,55,E,1,10,16);
		   }
		   else
		   {    
			   value_temp = value*10;
			   
			   no1_bit=(int)(value_temp/100);
			   value_temp = value_temp % 100;
        	   no2_bit=(int)(value_temp / 10);
			   
         	   no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db    
			 
			 display_hz(&space[0],13,55,14);		//删除之前的字符
			 display_img(13,55,key1[11],1,2,16);        //"-"     
			 
			 if(no1_bit==0)
			 { 
			   display_img(15,55,key1[no2_bit],1,2,16);  
				 display_hz(&dot[0],17,55,1);
				 display_img(18,55,key1[no3_bit],1,2,16);
				 //dis_db(0xbc,0x5d,data_db[no3_bit]);
				 display_hz(&chars_NPdBm[2],21,55,3);    
			 }
			else
			{
			  display_img(15,55,key1[no1_bit],1,2,16);  
				display_img(17,55,key1[no2_bit],1,2,16);
				display_hz(&dot[0],19,55,1);
				display_img(20,55,key1[no3_bit],1,2,16);  
				display_hz(&chars_NPdBm[2],23,55,3);   
			} 
		   }
	  } 
	  else if((RFacc_temp>=1590)&&(RFacc_temp<2175))      //-15dBm~0dBm   adjust 45mv/db    -40dBm__1000mv  
	  {
		   no1_bit=(int)(RFacc_temp-1590)%39;
		   no2_bit=(int)(RFacc_temp-1590-no1_bit)/39;   
		   no3_bit=(int)(no1_bit/3.9);
		   value=15-no2_bit-0.1*no3_bit; 
		   //if(value>30)
		   //{
			// display_hz(&space[0],13,55,14);
				//display_img(15,55,E,1,10,16); 
		  // }
		  // else
		  // {  
                   value_temp = value*10;
                   
                   no1_bit=(int)(value_temp/100);
                   value_temp = value_temp % 100;
        	   no2_bit=(int)(value_temp / 10);
			   
         	   no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db    
			 
                   display_hz(&space[0],13,55,14);		//删除之前的字符
                   //display_img(13,55,key1[11],1,2,16);        //"-"     
                   
                   if(no1_bit==0)
                   {
                     if(no2_bit == 0&&no3_bit == 0)
                     {
                       display_img(15,55,key1[no2_bit],1,2,16);  
                           display_hz(&dot[0],17,55,1);
                           display_img(18,55,key1[no3_bit],1,2,16);
                           //dis_db(0xbc,0x5d,data_db[no3_bit]);
                           display_hz(&chars_NPdBm[2],21,55,3);    
                     }
                     else
                     {
                        display_img(13,55,key1[11],1,2,16);        //"-"    
                        display_img(15,55,key1[no2_bit],1,2,16);  
                           display_hz(&dot[0],17,55,1);
                           display_img(18,55,key1[no3_bit],1,2,16);
                           //dis_db(0xbc,0x5d,data_db[no3_bit]);
                           display_hz(&chars_NPdBm[2],21,55,3);
                     }
                   }
                  else
                  {
                    display_img(13,55,key1[11],1,2,16);        //"-"  
                    display_img(15,55,key1[no1_bit],1,2,16);  
                          display_img(17,55,key1[no2_bit],1,2,16);
                          display_hz(&dot[0],19,55,1);
                          display_img(20,55,key1[no3_bit],1,2,16);  
                          display_hz(&chars_NPdBm[2],23,55,3);  
                  }
		   //}
	  } 
	  
	  else if((RFacc_temp>=2175)&&(RFacc_temp<2503))       //0~+8dBm     adjust 45mv/db         0dBm__2800mv
	  {
		no1_bit=(int)(RFacc_temp-2175)%41;
		no2_bit=(int)(RFacc_temp-2175-no1_bit)/41;
		no3_bit=(int)(no1_bit/4.1); 
		value=no2_bit+no3_bit*0.1;                    //4.5mv/0.1db      
		value_temp = value*10;
			   
                no1_bit=(int)(value_temp/100);
                value_temp = value_temp % 100;
                no2_bit=(int)(value_temp / 10);
                       
                no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
		
		display_hz(&space[0],13,55,14);		//删除之前的字符
		//display_img(13,55,key1[10],1,2,16); 
	   
		if(no1_bit==0) 
	        {
                  if(no2_bit == 0 && no3_bit == 0)
                  {
                    display_img(15,55,key1[no2_bit],1,2,16);  
                   display_hz(&dot[0],17,55,1);
                   display_img(18,55,key1[no3_bit],1,2,16);
                                   //dis_db(0xbc,0x5d,data_db[no3_bit]);
                   display_hz(&chars_NPdBm[2],21,55,3); 
                  }
                  else
                  {
                    display_img(13,55,key1[10],1,2,16);        //"+"  
                   display_img(15,55,key1[no2_bit],1,2,16);  
                   display_hz(&dot[0],17,55,1);
                   display_img(18,55,key1[no3_bit],1,2,16);
                                   //dis_db(0xbc,0x5d,data_db[no3_bit]);
                   display_hz(&chars_NPdBm[2],21,55,3);
                  }
	        } 
                 else
                 {
                   display_img(13,55,key1[10],1,2,16);        //"+"  
                       display_img(15,55,key1[no1_bit],1,2,16);  
                                      display_img(17,55,key1[no2_bit],1,2,16);
                                      display_hz(&dot[0],19,55,1);
                                      display_img(20,55,key1[no3_bit],1,2,16);  
                                      display_hz(&chars_NPdBm[2],23,55,3);  
                 }  
	  }    
	  else if((RFacc_temp>=2503)&&(RFacc_temp<2871))       //8~+16dBm     adjust 45mv/db         0dBm__2800mv
	  {
		no1_bit=(int)(RFacc_temp-2503)%46;
		no2_bit=(int)(RFacc_temp-2503-no1_bit)/46;
		no3_bit=(int)(no1_bit/4.6); 
		value=8+no2_bit+no3_bit*0.1;                    //4.5mv/0.1db      
		value_temp = value*10;
			   
	    no1_bit=(int)(value_temp/100);
	    value_temp = value_temp % 100;
	    no2_bit=(int)(value_temp / 10);
	   
	    no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
		
		display_hz(&space[0],13,55,14);		//删除之前的字符
		display_img(13,55,key1[10],1,2,16); 
	   
		if(no1_bit==0) 
	   	{
			display_img(15,55,key1[no2_bit],1,2,16);  
		    display_hz(&dot[0],17,55,1);
		    display_img(18,55,key1[no3_bit],1,2,16);
		   	//dis_db(0xbc,0x5d,data_db[no3_bit]);
		   	display_hz(&chars_NPdBm[2],21,55,3);
	   	} 
	   	else
	   	{ 
		   	display_img(15,55,key1[no1_bit],1,2,16);  
		   	display_img(17,55,key1[no2_bit],1,2,16);
	   	   	display_hz(&dot[0],19,55,1);
		   	display_img(20,55,key1[no3_bit],1,2,16);  
		   	display_hz(&chars_NPdBm[2],23,55,3); 
	   	} 
	  }    
	  else if((RFacc_temp>=2871)&&(RFacc_temp<3311))       //+16dBm~+24dBm   adjust 45mv/db            +10dBm__3250mv
	  {
		  dw_flag = eeprom_read(0x14);
		  no1_bit=(int)(RFacc_temp-2871)%55;	
		  no2_bit=(int)(RFacc_temp-2871-no1_bit)/55;
		  no3_bit=(int)(no1_bit/5.5);              //4.5mv/0.1db
		  value=16+no2_bit+no3_bit*0.1;
		  
		  if(value>20)                         //当射频值大于20时才允许改变单位
		  {
			  if(dw_flag == 0)       //单位是w
			  {
				RF_value = value/10;
				dbvalue_w = exp(RF_value*log(10));
				dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
				dbvalue_w += 0.05;
				value_temp = dbvalue_w*10;
			   
			    no1_bit=(int)(value_temp/100);
			    value_temp = value_temp % 100;
        	    no2_bit=(int)(value_temp / 10);			   
         	   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			   
				//if(refresh_w_flag == 1)
				//{
			   		display_hz(&space[0],13,55,14);		//删除之前的字符
				//	refresh_w_flag = 0;
				//}
				//else
				//{
				//	display_hz(&space[0],13,55,6);		//删除之前的字符
				//}
			   	//display_img(13,55,key1[10],1,2,16); 
			   
		 		display_img(16,55,key1[no2_bit],1,2,16);  
				display_hz(&dot[0],18,55,1);
				display_img(19,55,key1[no3_bit],1,2,16);
				//display_hz(&chars_NPdBm[5],21,55,1);
				display_img(21,55,key1[16],1,2,16);
     		}
			else                                //大于20  单位是dbm
			{
				value_temp = value*10;
			   
			   	no1_bit=(int)(value_temp/100);
			   	value_temp = value_temp % 100;
        	   	no2_bit=(int)(value_temp / 10);		   
         	   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
				
				display_hz(&space[0],13,55,14);		//删除之前的字符
				display_img(13,55,key1[10],1,2,16); 
				
				display_img(15,55,key1[no1_bit],1,2,16);  
				display_img(17,55,key1[no2_bit],1,2,16);
				display_hz(&dot[0],19,55,1);
				display_img(20,55,key1[no3_bit],1,2,16);  
				display_hz(&chars_NPdBm[2],23,55,3); 
				refresh_w_flag = 1;
			} 
		}
		else                                                  //小于20  显示为dbm形式
		{
			value_temp = value*10;
			   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);		   
			no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			
			display_hz(&space[0],13,55,14);		//删除之前的字符
			display_img(13,55,key1[10],1,2,16); 
			
			display_img(15,55,key1[no1_bit],1,2,16);  
			display_img(17,55,key1[no2_bit],1,2,16);
			display_hz(&dot[0],19,55,1);
			display_img(20,55,key1[no3_bit],1,2,16);  
			display_hz(&chars_NPdBm[2],23,55,3);   
			refresh_w_flag = 1;
		} 
	  }                
	  else if((RFacc_temp>=3311)&&(RFacc_temp<3655))       //+24dBm~+32dBm   adjust 45mv/db            +10dBm__3250mv
	  { 
		  dw_flag = eeprom_read(0x14);
		  no1_bit=(int)(RFacc_temp-3311)%43;
		  no2_bit=(int)(RFacc_temp-3311-no1_bit)/43;
		  no3_bit=(int)(no1_bit/4.3);              //4.5mv/0.1db
		  value=24+no2_bit+no3_bit*0.1;  
		
		  if(dw_flag == 0)       //单位是w
		  {
			RF_value = value/10;
			dbvalue_w = exp(RF_value*log(10));
			dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
			dbvalue_w += 0.05;
			value_temp = dbvalue_w*10;
		   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);			   
		   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
		   
		   	//if(refresh_w_flag == 1)
			//{
				display_hz(&space[0],13,55,14);		//删除之前的字符
			//	refresh_w_flag = 0;
			//}
			//else
			//{
			//	display_hz(&space[0],13,55,6);		//删除之前的字符
			//}
		   	//display_img(13,55,key1[10],1,2,16); 
		   
			display_img(16,55,key1[no2_bit],1,2,16);  
			display_hz(&dot[0],18,55,1);
			display_img(19,55,key1[no3_bit],1,2,16);
			//dis_db(0xbc,0x5d,data_db[no3_bit]);
			display_img(21,55,key1[16],1,2,16);				
		 }
		 else                                //大于20  单位是dbm
		 {
			value_temp = value*10;
		   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);		   
			no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			
			display_hz(&space[0],13,55,14);		//删除之前的字符
			display_img(13,55,key1[10],1,2,16); 
			
			display_img(15,55,key1[no1_bit],1,2,16);  
			display_img(17,55,key1[no2_bit],1,2,16);
			display_hz(&dot[0],19,55,1);
			display_img(20,55,key1[no3_bit],1,2,16);  
			display_hz(&chars_NPdBm[2],23,55,3);
			refresh_w_flag = 1;
		 } 
	  } 
	  else if((RFacc_temp>=3655)&&(RFacc_temp<3932))              //+32dBm~+40dBm   adjust 45mv/db
	  {
		  dw_flag = eeprom_read(0x14);
		  no1_bit=(int)(RFacc_temp-3655)%38;
		  no2_bit=(int)(RFacc_temp-3655-no1_bit)/38;
		  no3_bit=(int)(no1_bit/3.8);              //4.5mv/0.1db
		  value=32+no2_bit+no3_bit*0.1;  
     
		 if(value>40)
		 { 
			 display_hz(&space[0],13,55,14);
			 display_img(15,55,E,1,10,16);
		 
			 if(beiguang_off % 3 == 0)
			 {
				 for(m=0;m<2;m++)        //feng ming xiang 
				{
					for(j=0;j<3;j++)
					{
						for(n=0;n<0xff;n++)
						{ 
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf|0x20;  
							for(i=0;i<0x60;i++)  ;  
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf; 
							for(i=0;i<0x60;i++)  ; 
						}
					} 
					for(n=0;n<0xff;n++)    
					{ 
						DDRB=DDRB&0xdf|0x20;     
						PORTB=PORTB&0xdf;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
					}
				} 
			 }
		}
		else 
		{  
			if(dw_flag == 0)       //单位是w
			{	
				RF_value = value/10;
				dbvalue_w = exp(RF_value*log(10));
				dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
				dbvalue_w += 0.05;
				value_temp = dbvalue_w*10;
			   
				no1_bit=(int)(value_temp/100);
				value_temp = value_temp % 100;
				no2_bit=(int)(value_temp / 10);			   
			   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			   
			   	//if(refresh_w_flag == 1)
				//{
					display_hz(&space[0],13,55,14);		//删除之前的字符
					//refresh_w_flag = 0;
				//}
				//else
				//{
				//	display_hz(&space[0],13,55,6);		//删除之前的字符
				//}
			   	//display_img(13,55,key1[10],1,2,16); 
			   	if(no1_bit==0) 
			   	{
					display_img(16,55,key1[no2_bit],1,2,16);  
					display_hz(&dot[0],18,55,1);
					display_img(19,55,key1[no3_bit],1,2,16);
					display_img(21,55,key1[16],1,2,16);
			   	} 
			   	else
			   	{ 
					display_img(16,55,key1[no1_bit],1,2,16);  
					display_img(18,55,key1[no2_bit],1,2,16);
					//display_hz(&dot[0],19,55,1);
					//display_img(20,55,key1[no3_bit],1,2,16);  
					display_img(21,55,key1[16],1,2,16);
			   	}   
					
			}
			else                                //大于20  单位是dbm
			{
				value_temp = value*10;
			   
				no1_bit=(int)(value_temp/100);
				value_temp = value_temp % 100;
				no2_bit=(int)(value_temp / 10);		   
				no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
				
				display_hz(&space[0],13,55,14);		//删除之前的字符
				display_img(13,55,key1[10],1,2,16); 
				
				display_img(15,55,key1[no1_bit],1,2,16);  
				display_img(17,55,key1[no2_bit],1,2,16);
				display_hz(&dot[0],19,55,1);
				display_img(20,55,key1[no3_bit],1,2,16);  
				display_hz(&chars_NPdBm[2],23,55,3); 
				refresh_w_flag = 1;
			}     
		} 
	  }
	  else if(RFacc_temp>=3932)               
	  { 
		  display_hz(&space[0],13,55,14);
		  display_img(15,55,E,1,10,16);
		  
			 if(beiguang_off % 3 == 0)
			 {
				 for(m=0;m<2;m++)         //feng ming xiang 
				{
					for(j=0;j<3;j++)
					{
						for(n=0;n<0xff;n++)
						{ 
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf|0x20;  
							for(i=0;i<0x60;i++)  ;  
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf; 
							for(i=0;i<0x60;i++)  ; 
						}
					} 
					for(n=0;n<0xff;n++)    
					{ 
						DDRB=DDRB&0xdf|0x20;     
						PORTB=PORTB&0xdf;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
					}
				} 
			 }
		}
	}
	else if(flagUpDown == DOWN_GO_MARK)                               //xia xing
	{
		portd=PORTD;
		portd=portd&0x1F;    
		switch(portd)
		{ 
		case 0x00:               //+3           //2475
			RFacc_temp=RFacc_temp;
			break;
		case 0x01:          
			RFacc_temp=RFacc_temp-9;         //2440
			break;
		case 0x02:                                  //+1
			RFacc_temp=RFacc_temp-9;           //2395
			break; 
		case 0x03:                                  //0
			RFacc_temp=RFacc_temp-18;           //2350
			break;
		case 0x04:                                  //-1
			RFacc_temp=RFacc_temp;           //2305
			break; 
		case 0x05:                                  //-2
			RFacc_temp=RFacc_temp-9;           //2260
			break;
		case 0x06:                                 //-3
			RFacc_temp=RFacc_temp-9;          //2215mv
			break; 
		case 0x07:                                 //-4
			RFacc_temp=RFacc_temp-18;         //2170mv
			break;
		case 0x08: 
			RFacc_temp=RFacc_temp;          //-5    2125
			break; 
		case 0x09:                  
			RFacc_temp=RFacc_temp-9;        //-6  2080
			break; 
		case 0x0A: 
			RFacc_temp=RFacc_temp-9;          //-7    2035
			break;
		case 0x0B: 
			RFacc_temp=RFacc_temp-9;           //-8   1990
			break; 
		case 0x0C:             
			RFacc_temp=RFacc_temp+9;     //-9     1945
			break;
		case 0x0D: 
			RFacc_temp=RFacc_temp;      //-10   1900
			break; 
		case 0x0E: 
			RFacc_temp=RFacc_temp;      //-11   1855
			break;
		case 0x0F: 
			RFacc_temp=RFacc_temp;        //-12   1810
			break; 
		case 0x10:            
			RFacc_temp=RFacc_temp+9;      //-13   1765
			break;
		case 0x11:             
			RFacc_temp=RFacc_temp+9;        //-14   1720
			break; 
		case 0x12:             
			RFacc_temp=RFacc_temp+9;      //-15   1675
			break;
		case 0x13: 
			RFacc_temp=RFacc_temp;      //-16   1630
			break; 
		case 0x14:         
			RFacc_temp=RFacc_temp+18;        //-17   1585
			break;
		case 0x15: 
			RFacc_temp=RFacc_temp+9;        //-18   1540
			break; 
		case 0x16: 
			RFacc_temp=RFacc_temp+9;      //-19   1495
			break;
		case 0x17:         
			RFacc_temp=RFacc_temp;       //-20    1450
			break; 
		case 0x18: 
			RFacc_temp=RFacc_temp+18;       //-21    1405
			break;
		case 0x19: 
			RFacc_temp=RFacc_temp+9;     //-22    1360
			break; 
		case 0x1A: 
			RFacc_temp=RFacc_temp+9;       //-23    1315
			break;  
		case 0x1B:       
			RFacc_temp=RFacc_temp;       //-24    1270
			break;
		case 0x1C: 
			RFacc_temp=RFacc_temp+18;    //-25   1225
			break; 
		case 0x1D: 
			RFacc_temp=RFacc_temp+18;      //-26   1180
			break;
		case 0x1E: 
			RFacc_temp=RFacc_temp+13.5;      //-27   1135mv
			break; 
		case 0x1F:                //-28
			RFacc_temp=RFacc_temp;             //1090mv
			break;
		
		default:
			break; 
	 } 
         
         portd=PORTD;
		portd=portd&0x7F;
                switch(portd)
                {
                  case 0x20:
			RFacc_temp = RFacc_temp+9;          //-29
			break;
		  case 0x21:
			RFacc_temp = RFacc_temp+18;          //-30
			break;
                  case 0x22:
                        RFacc_temp = RFacc_temp;           //-31
                        break;
                  default:
                        break;
                }
		 if(RFacc_temp<965)/////////////////////display wrong   
		 {
			display_hz(&space[0],13,55,14);
			display_img(15,55,E,1,10,16); 
		 } 
	 
		else if((RFacc_temp>=965)&&(RFacc_temp<1565))      //-30dBm~-15dBm   adjust 49mv/db    -30dBm__1000mv  
		{
		   no1_bit=((int)(RFacc_temp-965))%40;
		   no2_bit=(int)(RFacc_temp-965-no1_bit)/40;   
		   no3_bit=(int)(no1_bit/4.0);
		   
		   value=30-no2_bit-(float)(0.1*no3_bit);
		   
		   if(value>30)
		   {
				display_hz(&space[0],13,55,14);
				display_img(15,55,E,1,10,16);
		   }
		   else
		   {    
			   value_temp = value*10;
			   
			   no1_bit=(int)(value_temp/100);
			   value_temp = value_temp % 100;
        	   no2_bit=(int)(value_temp / 10);
			   
         	   no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db    
			 
			   display_hz(&space[0],13,55,14);		//删除之前的字符
			   display_img(13,55,key1[11],1,2,16);        //"-"     
	
			   display_img(15,55,key1[no1_bit],1,2,16);  
			   display_img(17,55,key1[no2_bit],1,2,16);
			   display_hz(&dot[0],19,55,1);
			   display_img(20,55,key1[no3_bit],1,2,16);  
			   display_hz(&chars_NPdBm[2],23,55,3);   

		   }
	  	} 
	  else if((RFacc_temp>=1565)&&(RFacc_temp<2165))      //-15dBm~-0dBm   adjust 45mv/db    -40dBm__1000mv  
	  {
		   no1_bit=(int)(RFacc_temp-1565)%40;
		   no2_bit=(int)(RFacc_temp-1565-no1_bit)/40;   
		   no3_bit=(int)(no1_bit/4.0);
		   value=15-no2_bit-0.1*no3_bit; 
		   //if(value>30)
		   //{
			// display_hz(&space[0],13,55,14);
				//display_img(15,55,E,1,10,16); 
		   //}
		   //else
		   //{  
                   value_temp = value*10;
                   
                   no1_bit=(int)(value_temp/100);
                   value_temp = value_temp % 100;
        	   no2_bit=(int)(value_temp / 10);
			   
         	   no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db    
			 
                   display_hz(&space[0],13,55,14);		//删除之前的字符
                   //display_img(13,55,key1[11],1,2,16);        //"-"     
                   
                   if(no1_bit==0)
                   {
                     if(no2_bit == 0 && no3_bit == 0)
                     {
                       display_img(15,55,key1[no2_bit],1,2,16);  
                           display_hz(&dot[0],17,55,1);
                           display_img(18,55,key1[no3_bit],1,2,16);
                           //dis_db(0xbc,0x5d,data_db[no3_bit]);
                           display_hz(&chars_NPdBm[2],21,55,3);    
                     }
                     else
                     {
                        display_img(13,55,key1[11],1,2,16);        //"-"    
                        display_img(15,55,key1[no2_bit],1,2,16);  
                           display_hz(&dot[0],17,55,1);
                           display_img(18,55,key1[no3_bit],1,2,16);
                           //dis_db(0xbc,0x5d,data_db[no3_bit]);
                           display_hz(&chars_NPdBm[2],21,55,3);
                     }
                   }
                  else
                  {
                    display_img(13,55,key1[11],1,2,16);        //"-"  
                    display_img(15,55,key1[no1_bit],1,2,16);  
                          display_img(17,55,key1[no2_bit],1,2,16);
                          display_hz(&dot[0],19,55,1);
                          display_img(20,55,key1[no3_bit],1,2,16);  
                          display_hz(&chars_NPdBm[2],23,55,3);  
                  }
		     // }
	  } 
	 
	   else if((RFacc_temp>=2165)&&(RFacc_temp<2501))       //0~+8dBm     adjust 45mv/db         0dBm__2800mv
	   {
			no1_bit=(int)(RFacc_temp-2165)%42;
			no2_bit=(int)(RFacc_temp-2165-no1_bit)/42;
			no3_bit=(int)(no1_bit/4.2); 
			value=no2_bit+no3_bit*0.1;                    //4.5mv/0.1db      
			value_temp = value*10;
				   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);   
			no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			
			display_hz(&space[0],13,55,14);		//删除之前的字符
			//display_img(13,55,key1[10],1,2,16); 
		   
			if(no1_bit==0) 
	        {
                  if(no2_bit == 0 && no3_bit == 0)
                  {
                    display_img(15,55,key1[no2_bit],1,2,16);  
                   display_hz(&dot[0],17,55,1);
                   display_img(18,55,key1[no3_bit],1,2,16);
                                   //dis_db(0xbc,0x5d,data_db[no3_bit]);
                   display_hz(&chars_NPdBm[2],21,55,3); 
                  }
                  else
                  {
                    display_img(13,55,key1[10],1,2,16);        //"+"  
                   display_img(15,55,key1[no2_bit],1,2,16);  
                   display_hz(&dot[0],17,55,1);
                   display_img(18,55,key1[no3_bit],1,2,16);
                                   //dis_db(0xbc,0x5d,data_db[no3_bit]);
                   display_hz(&chars_NPdBm[2],21,55,3);
                  }
	        } 
                 else
                 {
                   display_img(13,55,key1[10],1,2,16);        //"+"  
                       display_img(15,55,key1[no1_bit],1,2,16);  
                                      display_img(17,55,key1[no2_bit],1,2,16);
                                      display_hz(&dot[0],19,55,1);
                                      display_img(20,55,key1[no3_bit],1,2,16);  
                                      display_hz(&chars_NPdBm[2],23,55,3);  
                 }  
	   }    
	   else if((RFacc_temp>=2501)&&(RFacc_temp<2845))       //8~+16dBm     adjust 45mv/db         0dBm__2800mv
	   {
		   no1_bit=(int)(RFacc_temp-2501)%43;
		   no2_bit=(int)(RFacc_temp-2501-no1_bit)/43;
		   no3_bit=(int)(no1_bit/4.3); 
		   value=8+no2_bit+no3_bit*0.1;                    //4.5mv/0.1db      
		   value_temp = value*10;
			   
	       no1_bit=(int)(value_temp/100);
	       value_temp = value_temp % 100;
	       no2_bit=(int)(value_temp / 10);
	   
	       no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
		
		   display_hz(&space[0],13,55,14);		//删除之前的字符
		   display_img(13,55,key1[10],1,2,16); 
	   
		   if(no1_bit==0) 
	   	   {
		   		display_img(15,55,key1[no2_bit],1,2,16);  
		   		display_hz(&dot[0],17,55,1);
		   		display_img(18,55,key1[no3_bit],1,2,16);
		   		display_hz(&chars_NPdBm[2],21,55,3);
	   		} 
	   		else
	   		{ 
		   		display_img(15,55,key1[no1_bit],1,2,16);  
		   		display_img(17,55,key1[no2_bit],1,2,16);
		   		display_hz(&dot[0],19,55,1);
		   		display_img(20,55,key1[no3_bit],1,2,16);  
		   		display_hz(&chars_NPdBm[2],23,55,3); 
	   		} 
	  }    
	  else if((RFacc_temp>=2845)&&(RFacc_temp<3285))       //+16dBm~+24dBm   adjust 45mv/db            +10dBm__3250mv
	  {
		  dw_flag = eeprom_read(0x14);
		  no1_bit=(int)(RFacc_temp-2845)%55;
		  no2_bit=(int)(RFacc_temp-2845-no1_bit)/55;
		  no3_bit=(int)(no1_bit/5.5);              //4.5mv/0.1db
		  value=16+no2_bit+no3_bit*0.1;  
		   
		  if(value>20)                         //当射频值大于20时才允许改变单位
		  {
			if(dw_flag == 0)       //单位是w
     		{
				RF_value = value/10;
				dbvalue_w = exp(RF_value*log(10));
				dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
				dbvalue_w += 0.05;
				value_temp = dbvalue_w*10;
			   
			    no1_bit=(int)(value_temp/100);
			    value_temp = value_temp % 100;
        	    no2_bit=(int)(value_temp / 10);			   
         	   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			   
			   	//if(refresh_w_flag == 1)
				//{
			   		display_hz(&space[0],13,55,14);		//删除之前的字符
					//refresh_w_flag = 0;
				//}
				//else
				//{
				//	display_hz(&space[0],13,55,6);		//删除之前的字符
				//}
			   	//display_img(13,55,key1[10],1,2,16); 
			   
		 		display_img(16,55,key1[no2_bit],1,2,16);  
				display_hz(&dot[0],18,55,1);
				display_img(19,55,key1[no3_bit],1,2,16);
				display_img(21,55,key1[16],1,2,16);				
     		}
			else                                //大于20  单位是dbm
			{
				value_temp = value*10;
			   
			   	no1_bit=(int)(value_temp/100);
			   	value_temp = value_temp % 100;
        	   	no2_bit=(int)(value_temp / 10);		   
         	   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
				
				display_hz(&space[0],13,55,14);		//删除之前的字符
				display_img(13,55,key1[10],1,2,16); 
				
				display_img(15,55,key1[no1_bit],1,2,16);  
				display_img(17,55,key1[no2_bit],1,2,16);
				display_hz(&dot[0],19,55,1);
				display_img(20,55,key1[no3_bit],1,2,16);  
				display_hz(&chars_NPdBm[2],23,55,3);
				refresh_w_flag = 1;
			} 
		}
		else                                                  //小于20  显示为dbm形式
		{
			value_temp = value*10;
			   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);		   
			no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			
			display_hz(&space[0],13,55,14);		//删除之前的字符
			display_img(13,55,key1[10],1,2,16); 
			
			display_img(15,55,key1[no1_bit],1,2,16);  
			display_img(17,55,key1[no2_bit],1,2,16);
			display_hz(&dot[0],19,55,1);
			display_img(20,55,key1[no3_bit],1,2,16);  
			display_hz(&chars_NPdBm[2],23,55,3);
			refresh_w_flag = 1;
		} 
	}  
	else if((RFacc_temp>=3285)&&(RFacc_temp<3653))       //+24dBm~+32dBm   adjust 45mv/db            +10dBm__3250mv
	{
		dw_flag = eeprom_read(0x14);
		no1_bit=(int)(RFacc_temp-3285)%46;
		no2_bit=(int)(RFacc_temp-3285-no1_bit)/46;
		no3_bit=(int)(no1_bit/4.6);              //4.5mv/0.1db
		value=24+no2_bit+no3_bit*0.1;  
			   
		if(dw_flag == 0)       //单位是w
		{
			RF_value = value/10;
			dbvalue_w = exp(RF_value*log(10));
			dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
			dbvalue_w += 0.05;
			value_temp = dbvalue_w*10;
		   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);			   
		   	no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
		   
		   	//if(refresh_w_flag == 1)
			//{
				display_hz(&space[0],13,55,14);		//删除之前的字符
				//refresh_w_flag = 0;
			//}
			//else
			//{
			//	display_hz(&space[0],13,55,6);		//删除之前的字符
			//}
		   	//display_img(13,55,key1[10],1,2,16); 
		   
			display_img(16,55,key1[no2_bit],1,2,16);  
			display_hz(&dot[0],18,55,1);
			display_img(19,55,key1[no3_bit],1,2,16);
			display_img(21,55,key1[16],1,2,16);				
		}
		else                                //大于20  单位是dbm
		{
			value_temp = value*10;
		   
			no1_bit=(int)(value_temp/100);
			value_temp = value_temp % 100;
			no2_bit=(int)(value_temp / 10);		   
			no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
			
			display_hz(&space[0],13,55,14);		//删除之前的字符
			display_img(13,55,key1[10],1,2,16); 
			
			display_img(15,55,key1[no1_bit],1,2,16);  
			display_img(17,55,key1[no2_bit],1,2,16);
			display_hz(&dot[0],19,55,1);
			display_img(20,55,key1[no3_bit],1,2,16);  
			display_hz(&chars_NPdBm[2],23,55,3);  
			refresh_w_flag = 1;
		} 
	}
  
	  else if((RFacc_temp>=3653)&&(RFacc_temp<3953))              //+32dBm~+40dBm   adjust 45mv/db
	  {
		  dw_flag = eeprom_read(0x14);
		  no1_bit=(int)(RFacc_temp-3653)%38;
		  no2_bit=(int)(RFacc_temp-3653-no1_bit)/38;
		  no3_bit=(int)(no1_bit/3.8);              //4.5mv/0.1db
		  value=32+no2_bit+no3_bit*0.1;   
			          
		  if(value>40)
		  { 
		   	display_hz(&space[0],13,55,14);
			display_img(15,55,E,1,10,16);
		  
			if(beiguang_off % 3 == 0)
			 {
				 for(m=0;m<2;m++)         //feng ming xiang 
				{
					for(j=0;j<3;j++)
					{
						for(n=0;n<0xff;n++)
						{ 
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf|0x20;  
							for(i=0;i<0x60;i++)  ;  
							DDRB=DDRB&0xdf|0x20;     
							PORTB=PORTB&0xdf; 
							for(i=0;i<0x60;i++)  ; 
						}
					} 
					for(n=0;n<0xff;n++)    
					{ 
						DDRB=DDRB&0xdf|0x20;     
						PORTB=PORTB&0xdf;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
						//for(i=0;i<0xff;i++)  ;
					}
				} 
			 }
			}
			else 
			{  
				if(dw_flag == 0)       //单位是w
				{	
					RF_value = value/10;
					dbvalue_w = exp(RF_value*log(10));
					dbvalue_w = dbvalue_w / 1000;          //取dbvalue_w的小数点后一位 还要四舍五入 显示
					dbvalue_w += 0.05;
					value_temp = dbvalue_w*10;
				   
					no1_bit=(int)(value_temp/100);
					value_temp = value_temp % 100;
					no2_bit=(int)(value_temp / 10);			   
					no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
				   
					//if(refresh_w_flag == 1)
					//{
						display_hz(&space[0],13,55,14);		//删除之前的字符
					//	refresh_w_flag = 0;
					//}
					//else
					//{
					//	display_hz(&space[0],13,55,6);		//删除之前的字符  d但不删除w  不抖动
					//}
					//display_img(13,55,key1[10],1,2,16); 
					if(no1_bit==0) 
					{
						display_img(16,55,key1[no2_bit],1,2,16);  
						display_hz(&dot[0],18,55,1);
						display_img(19,55,key1[no3_bit],1,2,16);
						display_img(21,55,key1[16],1,2,16);
					} 
					else
					{ 
						display_img(16,55,key1[no1_bit],1,2,16);  
						display_img(18,55,key1[no2_bit],1,2,16);
						//display_hz(&dot[0],19,55,1);
						//display_img(20,55,key1[no3_bit],1,2,16);  
						display_img(21,55,key1[16],1,2,16);
					}   
						
				}
				else                                //大于20  单位是dbm
				{
					value_temp = value*10;
				   
					no1_bit=(int)(value_temp/100);
					value_temp = value_temp % 100;
					no2_bit=(int)(value_temp / 10);		   
					no3_bit=(int)(value_temp % 10);  //4.5mv/0.1db
					
					display_hz(&space[0],13,55,14);		//删除之前的字符
					display_img(13,55,key1[10],1,2,16); 
					
					display_img(15,55,key1[no1_bit],1,2,16);  
					display_img(17,55,key1[no2_bit],1,2,16);
					display_hz(&dot[0],19,55,1);
					display_img(20,55,key1[no3_bit],1,2,16);  
					display_hz(&chars_NPdBm[2],23,55,3);    
					refresh_w_flag = 1;
				}     
			} 
	  	}
	  	else if(RFacc_temp>=3953)     
	  	{
		  display_hz(&space[0],13,55,14);
		  display_img(15,55,E,1,10,16);
		  if(beiguang_off % 3 == 0)
		  {
			 for(m=0;m<2;m++)         //feng ming xiang 
			 {
				for(j=0;j<3;j++)
				{
					for(n=0;n<0xff;n++)
					{ 
						DDRB=DDRB&0xdf|0x20;     
						PORTB=PORTB&0xdf|0x20;  
						for(i=0;i<0x60;i++)  ;  
						DDRB=DDRB&0xdf|0x20;     
						PORTB=PORTB&0xdf; 
						for(i=0;i<0x60;i++)  ; 
					 }
				 } 
				 for(n=0;n<0xff;n++)    
				 { 
					DDRB=DDRB&0xdf|0x20;     
					PORTB=PORTB&0xdf;
					for(i=0;i<0xff;i++)  ;
					for(i=0;i<0xff;i++)  ;
					for(i=0;i<0xff;i++)  ;
					for(i=0;i<0xff;i++)  ;
					for(i=0;i<0xff;i++)  ;
					//for(i=0;i<0xff;i++)  ;
					//for(i=0;i<0xff;i++)  ;
				 }
			  } 
		   }
	  	}
	}
}
  

#pragma vector = TIMER3_OVF_vect
__interrupt void timer3_ovf_interrupt(void)
{
	flag_ADRF = 1;				//标志是否采样射频
	flagtime++;                 //控制30s关机 响三次报警声后清0
	if(flagtime > 5000)
		flagtime = 0;		
	bgon_off++;					//1分钟未动按键则背光灭，10分钟未动按键则自动关机  按下任何键则清0
	if(bgon_off > 5000)
		bgon_off = 0;
	beiguang_off++;             //控制开机时背光亮3s自动关闭
	if(beiguang_off > 5000)
		beiguang_off = 0;
	TCNT3H = 0X1E;				//设置定时器初值 定时1s 比较准确
	TCNT3L = 0XFF;
}
