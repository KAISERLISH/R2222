#ifndef _LED_H
#define _LED_H

#include "param.h"

#define LED_GPIO_PORT    	GPIOA
#define LED_R_GPIO_PIN		GPIO_Pin_4	
#define LED_Y_GPIO_PIN		GPIO_Pin_5
#define LED_B_GPIO_PIN		GPIO_Pin_6	
#define LED_G_GPIO_PIN		GPIO_Pin_7	

#define LED_RED           GPIO_Pin_4
#define LED_YELLOW        GPIO_Pin_5
#define LED_BLUE          GPIO_Pin_6
#define LED_GREEN         GPIO_Pin_7

#define LED_RED_OFF  			GPIOA->BSRRL = GPIO_Pin_4		//输出高电平
#define LED_YELLOW_OFF  		GPIOA->BSRRL = GPIO_Pin_5
#define LED_BLUE_OFF    	    GPIOA->BSRRL = GPIO_Pin_6
#define LED_GREEN_OFF  			GPIOA->BSRRL = GPIO_Pin_7

#define LED_RED_ON   			GPIOA->BSRRH = GPIO_Pin_4		//输出低电平·
#define LED_YELLOW_ON   		GPIOA->BSRRH = GPIO_Pin_5
#define LED_BLUE_ON   			GPIOA->BSRRH = GPIO_Pin_6
#define LED_GREEN_ON   			GPIOA->BSRRH = GPIO_Pin_7

#define LED_RED_TOGGLE			GPIOA->ODR ^= GPIO_Pin_4		//toggle切换
#define LED_YELLOW_TOGGLE 	GPIOA->ODR ^= GPIO_Pin_5
#define LED_BLUE_TOGGLE			GPIOA->ODR ^= GPIO_Pin_6
#define LED_GREEN_TOGGLE		GPIOA->ODR ^= GPIO_Pin_7




#define LED_R_ON            GPIO_ResetBits(LED_GPIO_PORT,LED_R_GPIO_PIN);

#define LED_R_OFF           GPIO_SetBits(LED_GPIO_PORT,LED_R_GPIO_PIN);

#define LED_B_ON            GPIO_ResetBits(LED_GPIO_PORT,LED_B_GPIO_PIN);

#define LED_B_OFF           GPIO_SetBits(LED_GPIO_PORT,LED_B_GPIO_PIN);

#define LED_G_ON            GPIO_ResetBits(LED_GPIO_PORT,LED_G_GPIO_PIN);

#define LED_G_OFF           GPIO_SetBits(LED_GPIO_PORT,LED_G_GPIO_PIN);

#define LED_Y_ON            GPIO_ResetBits(LED_GPIO_PORT,LED_Y_GPIO_PIN);

#define LED_Y_OFF           GPIO_SetBits(LED_GPIO_PORT,LED_Y_GPIO_PIN);

void LED_Init(void);
void ledtask(void);
void LED_light_water(void);
#endif

