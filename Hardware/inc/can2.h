#ifndef _CAN2_H
#define _CAN2_H

#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "includes.h"

#include "param.h"
#include "tim3.h"
#include "DJmotor.h"
#include "beep.h"
#include "Zdrive.h"
#include "Tmotor.h"



/*------CAN初始化宏定义 使用宏定义的方法易于移植--------*/
#define CAN2_GPIO_PORT GPIOB
#define CAN2_RX_PIN GPIO_Pin_5
#define CAN2_TX_PIN GPIO_Pin_6
#define Rcc_AHB1_CAN_Gpio_Port RCC_AHB1Periph_GPIOB
#define CAN_AF_GpioSourePin_TX GPIO_PinSource6
#define CAN_AF_GpioSourePin_RX GPIO_PinSource5
/*----------------------------------------------*/


void CAN2_Init(void);
#endif
