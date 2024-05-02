#ifndef _CAN1_H
#define _CAN1_H

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "ucos_ii.h"
#include "tim3.h"
#include "beep.h"
#include "queue.h"
#include "param.h"
//#include "dj2006.h"

void CAN1_Init(void);
void AnswerMasterReady(void);
void ResponseSignalToMaster(void);
// void AnswerMasterError(void);

// void AnswerMaster(CanRxMsg rx_message);

void SignalPost_CAN1(uint8_t _num);
#endif
