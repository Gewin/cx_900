#include "iom128.h"
#include "ina90.h"

#include"usart.h"
#include"LCD_240160.h"

char rev_data[13] = {0};
volatile uint UART1RecCount = 0;
volatile uint flag,i;
uint UART0RecFlag = 1;
void init_usart0()
{
	UBRR0H = 0;
	UBRR0L = 23;
	//UCSR0A |= (1<<U2X0);
	UCSR0B |= (1 << RXEN0)|(1 << TXEN0)|(1 << RXCIE0);//接收使能，发送使能，接收中断开
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);              //写UCSRC寄存器,异步通信无校验，8位数据1位停止位	
}


void send_char(char data)
{
	while( !(UCSR0A & (1<<UDRE0)));//查询发送寄存器是否空
		UDR0 = data;                //将要发送的数据送入发送寄存器
	//display_img(15,118,E,1,10,16);
}

void send_list_char(char *s) 
{
	 //while (*s) 
	for(int i=0;i<13;i++) 
	{
	 	 send_char(*s); 
		 s++; 
	 } 
}
#pragma vector = USART0_RXC_vect
__interrupt void USART0_RXC_interrupt(void)
{
	uchar rectemp;
		rectemp = UDR0;			//将缓冲区数据移出

		/*判断起始符，该段代码在每次通讯过程中只执行一次*/
		if((rectemp == 0x7E) && (UART1RecCount == 0))			
		{
		 	rev_data[0] = rectemp;
			UART1RecCount ++;
			return;
		}
		/*接收数据位*/
		if(UART1RecCount > 0 && rectemp != 0x7E)
		{
		 	rev_data[UART1RecCount] = rectemp;
			UART1RecCount ++;
		}
		/*当接收到停止位时，接收数据完成，接收标志位置一，接收计数清零，等待下一次接收*/
		if(UART1RecCount > 0 && rectemp == 0x7E)
		{
		 	rev_data[UART1RecCount] = rectemp;
			UART0RecFlag = 1;
			
			/*处理字符串的截止符*/
			//UART1RecString[UART1RecCount + 1] = '\0';
			UART1RecCount = 0;
		}
		/*如果超过规定接收长度仍然没有收到结束符，那么将计数值清零，重新接收*/	
		if(UART1RecCount > 13)
		{
			UART1RecCount = 0;
			UART0RecFlag = 1;
		}	
	 /*temp2 = UDR0;
	 //send_char(temp);
	if(temp2 == 0x7E)
	{
		flag = 1;
		//Dis_int(12,1,flag,10);	
		
	}
	
	if(flag == 1)
	{
		//_delay_ms(1);
		rev_data[i] = temp2;		//把接收到的数据存入数组  
		i++;
		if(i == 13)
		{
			i = 0;
			flag = 0;		
		}
		else if(i == 13)
		{
			i=0;
			flag=0;
			for(int i = 0;i<13;i++)
			{
				rev_data[i]=0;
			}
		} 
	}  */
	//for(int i=0;i<10;i++);
	//send_char(rev_data[i-1]);	
	//}
	
}

