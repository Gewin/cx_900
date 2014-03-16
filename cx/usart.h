#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

extern char rev_data[13];
//extern volatile uchar temp;

void init_usart0();
void send_char(char data);
void send_list_char(char *s);
#pragma vector=USART0_RXC_vect
__interrupt void USART_RXC_Server(void);
