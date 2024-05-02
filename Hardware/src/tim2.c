#include "tim2.h"

/**
 * @brief 0.15ms定时中断计数
 */
void Tim2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef tim2_strcuture;
    tim2_strcuture.TIM_Period = 150 - 1;
    tim2_strcuture.TIM_Prescaler = 84 - 1;
    tim2_strcuture.TIM_CounterMode = TIM_CounterMode_Up;
    tim2_strcuture.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &tim2_strcuture);

    NVIC_InitTypeDef NVIC_tim2_structure;
    NVIC_tim2_structure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_tim2_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_tim2_structure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_tim2_structure.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_tim2_structure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

static int Count = 0;
static bool toggle = true;

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        if (toggle)
				{
					// 利用toggle分时发 保证最多同时发送零个防止堵
            CAN_QueueOut(CAN1, &CAN1_SendQueue); //  回复主控  0.3ms 一次

				}
        else
				{
            CAN_QueueOut(CAN2, &CAN2_SendQueue); // 控制机构 0.3ms 一次

				}
        if (Count % 5 == 0) //
				{
            CAN_QueueOut(CAN2, &ZDrive_SendQueue);

            Count = 0;
        }
        if (Count % 5 == 1) // 实现分时发送
        {
            CAN_QueueOut(CAN2, &DM_SendQueue);
        }
        toggle = !toggle;
        Count = Count < 999 ? Count + 1 : 0;

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

