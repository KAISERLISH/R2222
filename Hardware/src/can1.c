#include "can1.h"
void CAN1_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio_initstruct;
    CAN_InitTypeDef CAN_initstruct;
    CAN_FilterInitTypeDef CAN_filterinitstruct;
    NVIC_InitTypeDef NVIC_initstruct;

    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_initstruct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &gpio_initstruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

    CAN_DeInit(CAN1);                // Deinitializes the CAN peripheral registers to their default reset values.
    CAN_StructInit(&CAN_initstruct); // Fills each CAN_InitStruct member with its default value.

    /* CAN cell init */
    CAN_initstruct.CAN_TTCM = DISABLE; // 非时间触发通道模式
    CAN_initstruct.CAN_ABOM = ENABLE;  // 软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
    CAN_initstruct.CAN_AWUM = DISABLE; // 睡眠模式由软件唤醒
    CAN_initstruct.CAN_NART = DISABLE; // 禁止报文自动发送，即只发送一次，无论结果如何
    CAN_initstruct.CAN_RFLM = DISABLE; // 报文不锁定，新的覆盖旧的
    CAN_initstruct.CAN_TXFP = DISABLE; // 发送FIFO的优先级由标识符决定
    CAN_initstruct.CAN_Mode = CAN_Mode_Normal;

    /* Seting BaudRate */
    CAN_initstruct.CAN_SJW = CAN_SJW_1tq; // 重新同步跳跃宽度为一个时间单位
    CAN_initstruct.CAN_BS1 = CAN_BS1_9tq; // 时间段1占用8个时间单位
    CAN_initstruct.CAN_BS2 = CAN_BS2_4tq; // 时间段2占用7个时间单位
    CAN_initstruct.CAN_Prescaler = 3;     // 分频系数（Fdiv）
    CAN_Init(CAN1, &CAN_initstruct);      // 初始化CAN1
                                          //  can波特率=42M/（1+CAN_BS1+CAN_BS2）/CAN_Prescaler=42000/（1+9+4）/3 = 1Mbps
                                          //  CAN1通信波特率=42MHZ

    // 掩码模式
    CAN_filterinitstruct.CAN_FilterNumber = 0;
    CAN_filterinitstruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_filterinitstruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_filterinitstruct.CAN_FilterIdHigh = ((0x01010500 << 3) >> 16) & 0xffff;
    CAN_filterinitstruct.CAN_FilterIdLow = (0x01010500 << 3) & 0xffff;
    // 前面24位必须相同 只关心后的8位
    CAN_filterinitstruct.CAN_FilterMaskIdHigh = ((0xffffff00 << 3) >> 16) & 0xffff;
    CAN_filterinitstruct.CAN_FilterMaskIdLow = (0xffffff00 << 3) & 0xffff;
    CAN_filterinitstruct.CAN_FilterFIFOAssignment = CAN_FilterFIFO0; // 关联过滤器0到邮箱0
    CAN_filterinitstruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_filterinitstruct);

    // 列表模式
    CAN_filterinitstruct.CAN_FilterNumber = 1;
    CAN_filterinitstruct.CAN_FilterMode = CAN_FilterMode_IdList;
    CAN_filterinitstruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_filterinitstruct.CAN_FilterIdHigh = (0x01010000 << 5) >> 16;
    CAN_filterinitstruct.CAN_FilterIdLow = (0x01010000 << 5) & 0xffff;
    CAN_filterinitstruct.CAN_FilterFIFOAssignment = CAN_FilterFIFO0; // 关联过滤器0到邮箱0
    CAN_filterinitstruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_filterinitstruct);

    NVIC_initstruct.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_initstruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_initstruct);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

// 软件复位函数
void SystemReset(void)
{
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 进行软件复位
}
