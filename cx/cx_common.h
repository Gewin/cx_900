#ifndef _CX_COMMON_H
#define _CX_COMMON_H

#define UP_GO_MARK     8
#define DOWN_GO_MARK   3


#define GPIO_F_RELAY     0x02
#define GPIO_E_CHARGE    0x80

#define EEP_UNIT_FG                0x14
#define EEP_DBVALUE                0x15
#define EEP_DB_PLUS_MINUX_FG       0x16
#define EEP_DB_CHANGE_RANGE        0x17
#define EEP_CURRENT_CUR_LOCATION   0x18
#define EEP_RF_ON_OFF_FG           0x19
#define EEP_RF_FIRST_BIT           0x20
#define EEP_UP_DOWN_FG             0x21
#define EEP_RF_THREAD_BIT          0x22
#define EEP_RF_FOURTH_BIT          0x23
#define EEP_RF_FIFTH_BIT           0x24
#define EEP_RECENT_FAR_FG          0x25


typedef enum
{
    FAR_MACHINE = 0,
    RECENT_MACHINE,
}SYSTEM_MACHINE_ENUM;

#endif

