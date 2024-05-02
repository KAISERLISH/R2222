#include "led.h"
// LED IO 初始化

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                  // 定义结构体变量
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_R_GPIO_PIN | LED_Y_GPIO_PIN | LED_B_GPIO_PIN | LED_G_GPIO_PIN; // LED0 和 LED1 对应 IO 口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                                    // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                   // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                               // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                                     // 上拉
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);                                                   // 初始化 GPIO
    GPIO_SetBits(LED_GPIO_PORT, LED_R_GPIO_PIN | LED_Y_GPIO_PIN | LED_B_GPIO_PIN | LED_G_GPIO_PIN);  // 置位，灯灭
}

void ledtask(void)
{

    if (Led.Normal)
    {
        LED_light_water();
    }
    if (Led.Stuck)
    {
        LED_RED_TOGGLE; // 堵转
        Led.Stuck--;
    }
    if (Led.Timeout)
    {
        LED_YELLOW_TOGGLE; // 超时报警
        Led.Timeout--;
    }
    if (Led.CANErr)
    {
        LED_BLUE_TOGGLE;
        Led.CANErr--;
    }
}

void LED_light_water(void)
{
    LED_RED_TOGGLE;
    OSTimeDly(10000);
    LED_RED_TOGGLE;
    LED_BLUE_TOGGLE;
    OSTimeDly(10000);
    LED_BLUE_TOGGLE;
    LED_YELLOW_TOGGLE;
    OSTimeDly(10000);
    LED_YELLOW_TOGGLE;
    LED_GREEN_TOGGLE;
    OSTimeDly(10000);
    LED_GREEN_TOGGLE;
}
