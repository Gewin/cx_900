#define uchar unsigned char
#define uint unsigned int

#define REF_FRE              25

#define DOWN_GO_DEFAULT      932000
#define UP_GO_DEFAULT        887000

#define RF_BIT_DEFAULT       0

#define RF_DOWN_FIRST_BIT    9
#define RF_DOWN_SECOND_BIT   3
#define RF_DOWN_THREAD_BIT   2

#define RF_UP_FIRST_BIT      8
#define RF_UP_SECOND_BIT     8
#define RF_UP_THREAD_BIT     7

void InitADF4118Gpio();
void SetADF4118Reg(unsigned long int data);
void InitADF4118(unsigned long int Fvco, uint Ref);

