#ifndef _CYLINDER_H
#define _CYLINDER_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define Set_CYLDATA() 	GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define Reset_CYLDATA()	GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#define Set_CYLCLK() 		GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define Reset_CYLCLK() 	GPIO_ResetBits(GPIOA, GPIO_Pin_10)

/* ----------------------------- Decleration ----------------------------- */

void Valve_Init(void);
void Valve_Ctrl(u8 data);

/* ----------------------------- End of file ----------------------------- */


#endif
