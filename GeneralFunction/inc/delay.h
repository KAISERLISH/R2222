#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"
#include "stdbool.h"

void Delay_ms(unsigned int t);
void Delay_us(unsigned int t);
void Delay(u16 t);
void delay_us(u32 nus);
void delay_ms(u32 nms);
void delay_s(int n);

#endif
