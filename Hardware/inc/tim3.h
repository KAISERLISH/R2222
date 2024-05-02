#ifndef _TIM3_H
#define _TIM3_H
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

#include "DJmotor.h"
#include "zdrive.h"

#define USE_DJ_tim 1
#define USE_VESC_tim 1
#define USE_ELMO_tim 0

void Tim3_Init(void);

#endif
