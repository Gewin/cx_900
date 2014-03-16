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
	UCSR0B |= (1 << RXEN0)|(1 << TXEN0)|(1 << RXCIE0);//����ʹ�ܣ�����ʹ�ܣ������жϿ�
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);              //дUCSRC�Ĵ���,�첽ͨ����У�飬8λ����1λֹͣλ	
}


void send_char(char data)
{
	while( !(UCSR0A & (1<<UDRE0)));//��ѯ���ͼĴ����Ƿ��
		UDR0 = data;                //��Ҫ���͵��������뷢�ͼĴ���
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
		rectemp = UDR0;			//�������������Ƴ�

		/*�ж���ʼ�����öδ�����ÿ��ͨѶ������ִֻ��һ��*/
		if((rectemp == 0x7E) && (UART1RecCount == 0))			
		{
		 	rev_data[0] = rectemp;
			UART1RecCount ++;
			return;
		}
		/*��������λ*/
		if(UART1RecCount > 0 && rectemp != 0x7E)
		{
		 	rev_data[UART1RecCount] = rectemp;
			UART1RecCount ++;
		}
		/*�����յ�ֹͣλʱ������������ɣ����ձ�־λ��һ�����ռ������㣬�ȴ���һ�ν���*/
		if(UART1RecCount > 0 && rectemp == 0x7E)
		{
		 	rev_data[UART1RecCount] = rectemp;
			UART0RecFlag = 1;
			
			/*�����ַ����Ľ�ֹ��*/
			//UART1RecString[UART1RecCount + 1] = '\0';
			UART1RecCount = 0;
		}
		/*��������涨���ճ�����Ȼû���յ�����������ô������ֵ���㣬���½���*/	
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
		rev_data[i] = temp2;		//�ѽ��յ������ݴ�������  
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

