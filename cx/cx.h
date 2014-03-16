#define   KEY_C     10
#define   KEY_MHz   11
#define   KEY_LEFT  12
#define   KEY_UP    13
#define   KEY_RIGHT 14
#define   KEY_DOWN  15
#define   KEY_RF    16
#define   KEY_DOWNGO 17
#define   KEY_UPGO   18
#define   KEY_BEIGUANG 19
#define   KEY_RECENT  20
#define   KEY_FAR     21
#define   KEY_dBm_W   22
#define  EL_EN    PORTD=PORTD|0x80
#define  EL_CLOSE PORTD=PORTD&0x7f
#define  TX_UNEN  PORTB=PORTB&0xbf 
#define  TX_EN    PORTB=PORTB|0x40  
#define  CUT_RF_POWER  PORTD=PORTD&0x9f

#define uchar unsigned char
#define uint unsigned int

extern void eeprom_write(unsigned int add,unsigned char data);
extern unsigned char eeprom_read(unsigned int add);



