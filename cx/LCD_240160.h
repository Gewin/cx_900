 #define uchar unsigned char
#define uint unsigned int

void lcd_cmdwrite (uchar cmdx , uchar cmdy);
void lcd_datawrite(uchar datax);
void lcd_init(void);
void display_hz(uchar __flash *text,uchar x,uchar y,uchar count);
void display_img(uchar col,uchar row,uchar __flash *p,uchar c,uchar width,uchar high);
void display_wdian(uchar s,uchar n);
void lcd_clear(void);
void display_freq(void);
void display1_freq(void);
void delay(unsigned int t);
void eeprom_write(unsigned int add,unsigned char data);
unsigned char eeprom_read(unsigned int add);

extern uchar __flash main_interface1[];
extern uchar __flash chars_F[];
extern uchar __flash chars_freq[];
extern uchar __flash chars_1310nm[];
extern uchar __flash chars_1550nm[];
extern uchar __flash chars_dBm[];
extern uchar __flash battery[5][48];

extern uchar __flash Mode1[];
extern uchar __flash Mode2[];
extern uchar __flash Mode3[];
extern uchar __flash Mode4[];
extern uchar __flash Mode5[];
extern uchar __flash E[];

extern uchar __flash space[];
extern uchar __flash chars_NPdBm[];
extern uchar __flash key[];
extern uchar __flash dot[];
extern uchar __flash chars_MHz[];
extern uchar __flash key1[17][32];
extern uchar __flash key[];
