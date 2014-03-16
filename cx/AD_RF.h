#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

extern volatile long int flagtime,bgon_off,beiguang_off;
extern double vcc;
extern volatile uint flag_ADRF,flag_Usart;

extern volatile float value,value_W;

extern volatile uchar dw_flag;    //单位标志  1为dbm
extern volatile uchar flagUpDown;
extern float RF_acc,Battery_acc;

void Battery_Dis();
void RF_Dis();


#pragma vector = TIMER3_OVF_vect
__interrupt void timer3_ovf_interrupt(void);


