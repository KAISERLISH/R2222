#ifndef _PARAM_H
#define _PARAM_H
#include "stm32f4xx.h"
#include "stdbool.h"
#include "Zdrive.h"
#include "DJmotor.h"
#include "Tmotor.h"

typedef struct
{

    volatile bool CAN1_ControlList_Enable;
    volatile bool CAN2_ControlList_Enable;
    volatile bool CAN1SendQueueFULL;
    volatile bool CAN2SendQueueFULL;
    volatile bool CANSendQueueEMPTY;

    // TODO: 根据实际需要添加标志位
} FlagType;

typedef struct
{
    volatile bool AllowFlag;
    uint16_t BeepOnNum;
    volatile bool error;
    uint8_t error_cnt;
} BeepStructType;

typedef struct
{
    volatile bool Normal;
    volatile bool Stuck;
    volatile bool CANErr;
    volatile bool Timeout;
    volatile bool CloseAll;
} LEDStructType;

extern FlagType Flag;
extern BeepStructType Beep;
extern LEDStructType Led;
extern FlagType flag;
//extern Usart_StructType usart;

void param_Init(void);

#endif
