#include "Tmotor.h"	//AK电机

static CanTxMsg AKtx_message;
TMotorTypedef Tmotor[1];	//AK电机类

TmotorParamTypedef AK60G6Param;	//电机种类参数，包含减速比和齿数比

void Tmotor_Init(void)	//电机初始化
{
	AK60G6Param.angle2RAD = 0.017453293f ;		//角度到弧度
	AK60G6Param.RATIO = 6.0f;		//AK60-6自带减速比
	AK60G6Param.GearRadio = 1.0f;		//机构减速比
	Tmotor[0].param = AK60G6Param; // AK60，ID为1


}

void Tmotor_Comm_Init(CAN_TypeDef *CANx, u8 ID)	//通讯初始化
{
	CanTxMsg tx_message;
	tx_message.StdId = 0x00 + ID;
	tx_message.RTR = CAN_RTR_Data;	  //数据帧
	tx_message.IDE = CAN_Id_Standard; //标准帧
	tx_message.DLC = 8;
	tx_message.Data[0] = 0xFF;
	tx_message.Data[1] = 0xFF;
	tx_message.Data[2] = 0xFF;
	tx_message.Data[3] = 0xFF;

	tx_message.Data[4] = 0xFF;
	tx_message.Data[5] = 0xFF;
	tx_message.Data[6] = 0xFF;
	tx_message.Data[7] = 0xFC;
	CAN_Transmit(CANx, &tx_message); //发送
}

u16 float2uint(float x, float x_min, float x_max, u8 bits)	//浮点型化为整型
{
	float span = x_max - x_min;
	float offset = x_min;

	return (u16)((x - offset) * ((float)((1 << bits) - 1)) / span);
}
float uint2float(int x_int, float x_min, float x_max, int bits)	//整型化为浮点型
{
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

void Tmotor_Control(u8 ID, float f_pos, float f_vel, float f_kp, float f_kd, float f_torque)	//将控制参数传给电机，相当于大疆的DJ_CurrentTransmit																																																										//利用该函数分别实现位置控制或速度控制
{
	u16 p, v, kp, kd, t;
	f_pos *= Tmotor[ID-1].param.angle2RAD * Tmotor[ID-1].param.GearRadio * Tmotor[ID-1].param.RATIO;
	f_vel *= Tmotor[ID-1].param.angle2RAD * Tmotor[ID-1].param.GearRadio * Tmotor[ID-1].param.RATIO;
	/* 根据协议，对float参数进行转换 */
	p = float2uint(f_pos, P_MIN, P_MAX, 16);
	v = float2uint(f_vel, V_MIN, V_MAX, 12);
	kp = float2uint(f_kp, KP_MIN, KP_MAX, 12);
	kd = float2uint(f_kd, KD_MIN, KD_MAX, 12);
	t = float2uint(f_torque, T_MIN, T_MAX, 12);
	
    
	
  	AKtx_message.StdId = 0x00 + ID;
  	AKtx_message.IDE = CAN_Id_Standard; // 标准帧
 	AKtx_message.RTR = CAN_RTR_DATA; // 数据帧
 	AKtx_message.DLC = 8;
	AKtx_message.Data[0] = p >> 8;
	AKtx_message.Data[1] = p & 0xFF;
	AKtx_message.Data[2] = v >> 4;
	AKtx_message.Data[3] = ((v & 0xF) << 4) | (kp >> 8);
	AKtx_message.Data[4] = kp & 0xFF;
	AKtx_message.Data[5] = kd >> 4;
	AKtx_message.Data[6] = ((kd & 0xF) << 4) | (t >> 8);
	AKtx_message.Data[7] = t & 0xff;

    CAN_Transmit(CAN2, &AKtx_message);

}



void Tmotor_SetZero(u8 ID)	//设置当前位置为零位置
{
		Tmotor[ID].valueSet.angle=0;
		AKtx_message.StdId = 0x00 + ID;
		AKtx_message.DLC = 0x08;
		AKtx_message.Data[0] = 0xFF;
		AKtx_message.Data[1] = 0xFF;
		AKtx_message.Data[2] = 0xFF;
		AKtx_message.Data[3] = 0xFF;
		AKtx_message.Data[4] = 0xFF;
		AKtx_message.Data[5] = 0xFF;
		AKtx_message.Data[6] = 0xFF;
		AKtx_message.Data[7] = 0xFE;

		CAN_Transmit(CAN2, &AKtx_message);
}

void TmotorFunc(void)
{
	if (Tmotor[0].SetZero)
	{
		Tmotor_SetZero(TmotorID);
		Tmotor[0].SetZero = 0;
	}

	if (Tmotor[0].enable) //在这里打断点,为啥只有0和1？
	{
		switch (Tmotor[TmotorID-1].mode)
		{
			case TPos:	//位置模式。位置取决于valueSet.angle
				Tmotor_Control(TmotorID, Tmotor[0].valueSet.angle, 0, 0.5, \
				0, 0 );
				break;
			case TVel:	//速度模式。速度取决于valueSet.speed
				Tmotor_Control(TmotorID, 0, Tmotor[0].valueSet.speed, 0, Tmotor[0].valueSet.kd, \
				0);
				break;
			case TPosVel:
				Tmotor_Control(TmotorID, Tmotor[0].valueSet.angle, Tmotor[0].valueSet.speed,\
				Tmotor[0].valueSet.kp, Tmotor[0].valueSet.kd,Tmotor[0].valueSet.torque );
				break;
			default:
				break;
		}
	}
	else
	{
		Tmotor_Control(TmotorID, 0, 0, 0, 0, 0);
	}
}

void TmotorReceiveHandler(CanRxMsg rx_message)	//将电机传来的参数做处理得到电机的真实数据valueReal
{
	int id = rx_message.Data[0]-1;
	u16 p_int = (rx_message.Data[1] << 8) | rx_message.Data[2];		   //电机位置
	u16 v_int = (rx_message.Data[3] << 4) | (rx_message.Data[4] >> 4); //电机速度
	u16 t_int = (rx_message.Data[4] & 0x0f) << 8 | rx_message.Data[5]; //电机扭矩

	float p = uint2float(p_int, P_MIN, P_MAX, 16);
	float v = uint2float(v_int, V_MIN, V_MAX, 12);
	Tmotor[id].valueReal.Preanlge=Tmotor[id].valueReal.angle;
	Tmotor[id].valueReal.angle = (float)(p / Tmotor[id].param.angle2RAD / Tmotor[id].param.GearRadio / Tmotor[id].param.RATIO);
	Tmotor[id].valueReal.speed = (float)(v / Tmotor[id].param.angle2RAD / Tmotor[id].param.GearRadio / Tmotor[id].param.RATIO);
	Tmotor[id].valueReal.torque = uint2float(t_int, T_MIN, T_MAX, 12);
	
}
