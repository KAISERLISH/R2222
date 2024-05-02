#include "tim3.h"

void Tim3_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM3_TimeBaseInitStru;
    TIM3_TimeBaseInitStru.TIM_Period = 1000 - 1;
    TIM3_TimeBaseInitStru.TIM_Prescaler = 84 - 1;
    TIM3_TimeBaseInitStru.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_TimeBaseInitStru.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseInitStru);

    NVIC_InitTypeDef NVIC_TIM3Structure;
    NVIC_TIM3Structure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_TIM3Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_TIM3Structure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_TIM3Structure.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_TIM3Structure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);      // clears the TIM3's pending flag
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // enables or disables the specified TIM interrupts
    TIM_Cmd(TIM3, ENABLE);                     // enables or disables the specified TIM peripheral
}


static int ZdriveCount = 0;
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TmotorFunc();
        
        DJFunction();
        //readValuePack(&ConPack);

        if (ZdriveCount++ >= 10) //
        {
            ZdriveCount = 0;
            //ResponseMaster.unswer_zdrive_status++;

            ZdriveFunction();
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志
    }
}
