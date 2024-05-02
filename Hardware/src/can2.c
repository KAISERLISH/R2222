#include "can2.h"

void CAN2_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_FilterInitTypeDef CAN_Filter_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    RCC_AHB1PeriphClockCmd(Rcc_AHB1_CAN_Gpio_Port, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = CAN2_RX_PIN | CAN2_TX_PIN;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(CAN2_GPIO_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(CAN2_GPIO_PORT, CAN_AF_GpioSourePin_RX, GPIO_AF_CAN2); // 初始化GPIO的复用功能
    GPIO_PinAFConfig(CAN2_GPIO_PORT, CAN_AF_GpioSourePin_TX, GPIO_AF_CAN2);

    CAN_InitStructure.CAN_TTCM = DISABLE;         // 非时间触发通道模式
    CAN_InitStructure.CAN_ABOM = ENABLE;         // 软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
    CAN_InitStructure.CAN_AWUM = DISABLE;         // 睡眠模式由软件唤醒
    CAN_InitStructure.CAN_NART = DISABLE;         // 禁止报文自动发送，即只发送一次，无论结果如何
    CAN_InitStructure.CAN_RFLM = DISABLE;         // 报文不锁定，新的覆盖旧的
    CAN_InitStructure.CAN_TXFP = DISABLE;         // 发送FIFO的优先级由标识符决定
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // CAN硬件工作在正常模式

    /* Seting BaudRate */
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; // 重新同步跳跃宽度为一个时间单位
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; // 时间段1占用8个时间单位
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; // 时间段2占用7个时间单位
    CAN_InitStructure.CAN_Prescaler = 3;     // 分频系数（Fdiv）
    CAN_Init(CAN2, &CAN_InitStructure);      // 初始化CAN1
                                             //  can_BaudRate=42M/（1+CAN_BS1+CAN_BS2）/CAN_Prescaler=42000/（1+9+4）/3 = 1Mbps
    // F4的CAN1和CAN2在低速APB1上，系统频率为42MHZ.高速时钟APB2为84MHZ.且F1和F4时钟的频率是不一样的

    /*------------------CAN FIFO DJMOTOR----------------------*/
    CAN_Filter_InitStructure.CAN_FilterNumber = 14;                   // 选择14 参考README.md补充款第一条
    CAN_Filter_InitStructure.CAN_FilterMode = CAN_FilterMode_IdList;  // 列表模式
    CAN_Filter_InitStructure.CAN_FilterScale = CAN_FilterScale_16bit; // 16位过滤器
    CAN_Filter_InitStructure.CAN_FilterIdHigh = 0x201 << 5;
    CAN_Filter_InitStructure.CAN_FilterIdLow = 0x202 << 5;
    CAN_Filter_InitStructure.CAN_FilterMaskIdHigh = 0x203 << 5;
    CAN_Filter_InitStructure.CAN_FilterMaskIdLow = 0x204 << 5;
    CAN_Filter_InitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_Filter_InitStructure.CAN_FilterActivation = ENABLE; // 激活FIFO0
    CAN_FilterInit(&CAN_Filter_InitStructure);

    CAN_Filter_InitStructure.CAN_FilterNumber = 15;
    CAN_Filter_InitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
    CAN_Filter_InitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    CAN_Filter_InitStructure.CAN_FilterIdHigh = 0x205 << 5;
    CAN_Filter_InitStructure.CAN_FilterIdLow = 0x206 << 5;
    CAN_Filter_InitStructure.CAN_FilterMaskIdHigh = 0x207 << 5;
    CAN_Filter_InitStructure.CAN_FilterMaskIdLow = 0x208 << 5;
    CAN_Filter_InitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
    CAN_Filter_InitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_Filter_InitStructure);


    //**********Zdrive**********//
	CAN_Filter_InitStructure.CAN_FilterNumber = 16;
	CAN_Filter_InitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_Filter_InitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_Filter_InitStructure.CAN_FilterIdHigh = 0x04 << 5;
	CAN_Filter_InitStructure.CAN_FilterIdLow = 0x03 << 5;
	CAN_Filter_InitStructure.CAN_FilterMaskIdHigh = 0x02 << 5;
	CAN_Filter_InitStructure.CAN_FilterMaskIdLow = 0x01 << 5;
	CAN_Filter_InitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_Filter_InitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_Filter_InitStructure);

    /*-------------------------------------------*/
    CAN_Filter_InitStructure.CAN_FilterNumber = 24;
    CAN_Filter_InitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_Filter_InitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
    // 模式
    CAN_Filter_InitStructure.CAN_FilterIdLow = (0x054) << 5;
    CAN_Filter_InitStructure.CAN_FilterMaskIdLow = (0x0FF) << 5;
    CAN_Filter_InitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
    CAN_Filter_InitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_Filter_InitStructure);
    

    /*-------------------------------------------*/
    /*-------------------------------------------*/


    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
}

/**
 * @brief CAN2 FIFO0报文接收中断函数
 * 其中注意函数CAN_GetFlagStatus() 的参数 CAN_IT_FMP0 是挂起标志
 * CAN_FLAG_FMP0: FIFO 0 Message Pending Flag
 */
void CAN2_RX0_IRQHandler(void)
{

    if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
    {
        CanRxMsg Rx_message0;
        CAN_Receive(CAN2, CAN_FIFO0, &Rx_message0);
        DJ_ReceiveData_CAN2(Rx_message0); // 大疆
        // Flag.ScopeStartFlag=true;
    }
}

void CAN2_RX1_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2, CAN_IT_FMP1) != RESET)
    {
        CanRxMsg Rx_message1;
        CAN_Receive(CAN2, CAN_FIFO1, &Rx_message1);

        if ((Rx_message1.IDE == CAN_ID_STD) && (Rx_message1.RTR == CAN_RTR_DATA)) // IDE
        {
            if(Rx_message1.StdId==TmotorID)
                TmotorReceiveHandler(Rx_message1);
            else
                ZdriveReceiveHandler(Rx_message1);
        }
    }
}
