#ifndef _KEYANSWER1_H
#define _KEYANSWER1_H

#define   KEY_C     13
#define   KEY_MHz   11
#define   KEY_LEFT  24
#define   KEY_UP    19
#define   KEY_RIGHT 14
#define   KEY_DOWN  15
#define   KEY_RF    25
#define   KEY_DOWNGO 16
#define   KEY_UPGO   18
#define   KEY_BEIGUANG 20
#define   KEY_RECENT  23
#define   KEY_FAR     21
#define   KEY_dBm_W   22
#define  EL_EN    PORTD=PORTD|0x80
#define  EL_CLOSE PORTD=PORTD&0x7f
#define  TX_UNEN  PORTB=PORTB&0xbf 
#define  TX_EN    PORTB=PORTB|0x40  
#define  CUT_RF_POWER  PORTD=PORTD&0x9f

#define uchar unsigned char
#define uint unsigned int
#define _BV(bit) (1 << (bit))

extern volatile char RF_input[10];
extern volatile char RF_output[10];
extern volatile char Light_input[13];
extern volatile char Light_output[10];

extern volatile double AD_RF;
extern float dbvalue_w;

void WriteCur(unsigned char index,char step);          //记录按下当前按键应对应哪个位置
void KeyUp(void);                //等待按键抬起
void clearbit(unsigned char index);
void KeyAnswer(void);
void LeftKey(void);
void RightKey(void);
void DownKey(void);
void UpKey(void);
void putoutcode(unsigned char dbvalue);
uchar KeyScan(void);             //键盘扫描
float ADport(uchar port);
//double pow(double m,double n);

#endif
