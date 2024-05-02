#include "cylinder.h"

#include "delay.h"

void Valve_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	Set_CYLCLK();		// PA1=1
	Reset_CYLDATA();	// PA0=0，都开灯
}

/**
 * @brief: Valve control by bits
 */
void Valve_Ctrl(u8 Data)
{
	u8 i;
	if (Data > 0xFF)
		Data = 0XFF;
	for (i = 0; i < 8; i++) {
		Reset_CYLCLK();//PA1=0
		if (Data & 0x01)		// 如果数据最后一位为1
			Set_CYLDATA();		// PA0=1
		else					// 如果为0
			Reset_CYLDATA();	// PA0=0
		Set_CYLCLK();			// 打开时钟，PA1=1
		Data >>= 1;				// 右移一位
	}
	Set_CYLDATA();//PA0=1
}
