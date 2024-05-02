#include "Tmotor.h"	//AK���

static CanTxMsg AKtx_message;
TMotorTypedef Tmotor[1];	//AK�����

TmotorParamTypedef AK60G6Param;	//�������������������ٱȺͳ�����

void Tmotor_Init(void)	//�����ʼ��
{
	AK60G6Param.angle2RAD = 0.017453293f ;		//�Ƕȵ�����
	AK60G6Param.RATIO = 6.0f;		//AK60-6�Դ����ٱ�
	AK60G6Param.GearRadio = 1.0f;		//�������ٱ�
	Tmotor[0].param = AK60G6Param; // AK60��IDΪ1


}

void Tmotor_Comm_Init(CAN_TypeDef *CANx, u8 ID)	//ͨѶ��ʼ��
{
	CanTxMsg tx_message;
	tx_message.StdId = 0x00 + ID;
	tx_message.RTR = CAN_RTR_Data;	  //����֡
	tx_message.IDE = CAN_Id_Standard; //��׼֡
	tx_message.DLC = 8;
	tx_message.Data[0] = 0xFF;
	tx_message.Data[1] = 0xFF;
	tx_message.Data[2] = 0xFF;
	tx_message.Data[3] = 0xFF;

	tx_message.Data[4] = 0xFF;
	tx_message.Data[5] = 0xFF;
	tx_message.Data[6] = 0xFF;
	tx_message.Data[7] = 0xFC;
	CAN_Transmit(CANx, &tx_message); //����
}

u16 float2uint(float x, float x_min, float x_max, u8 bits)	//�����ͻ�Ϊ����
{
	float span = x_max - x_min;
	float offset = x_min;

	return (u16)((x - offset) * ((float)((1 << bits) - 1)) / span);
}
float uint2float(int x_int, float x_min, float x_max, int bits)	//���ͻ�Ϊ������
{
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

void Tmotor_Control(u8 ID, float f_pos, float f_vel, float f_kp, float f_kd, float f_torque)	//�����Ʋ�������������൱�ڴ󽮵�DJ_CurrentTransmit																																																										//���øú����ֱ�ʵ��λ�ÿ��ƻ��ٶȿ���
{
	u16 p, v, kp, kd, t;
	f_pos *= Tmotor[ID-1].param.angle2RAD * Tmotor[ID-1].param.GearRadio * Tmotor[ID-1].param.RATIO;
	f_vel *= Tmotor[ID-1].param.angle2RAD * Tmotor[ID-1].param.GearRadio * Tmotor[ID-1].param.RATIO;
	/* ����Э�飬��float��������ת�� */
	p = float2uint(f_pos, P_MIN, P_MAX, 16);
	v = float2uint(f_vel, V_MIN, V_MAX, 12);
	kp = float2uint(f_kp, KP_MIN, KP_MAX, 12);
	kd = float2uint(f_kd, KD_MIN, KD_MAX, 12);
	t = float2uint(f_torque, T_MIN, T_MAX, 12);
	
    
	
  	AKtx_message.StdId = 0x00 + ID;
  	AKtx_message.IDE = CAN_Id_Standard; // ��׼֡
 	AKtx_message.RTR = CAN_RTR_DATA; // ����֡
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



void Tmotor_SetZero(u8 ID)	//���õ�ǰλ��Ϊ��λ��
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

	if (Tmotor[0].enable) //�������ϵ�,Ϊɶֻ��0��1��
	{
		switch (Tmotor[TmotorID-1].mode)
		{
			case TPos:	//λ��ģʽ��λ��ȡ����valueSet.angle
				Tmotor_Control(TmotorID, Tmotor[0].valueSet.angle, 0, 0.5, \
				0, 0 );
				break;
			case TVel:	//�ٶ�ģʽ���ٶ�ȡ����valueSet.speed
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

void TmotorReceiveHandler(CanRxMsg rx_message)	//����������Ĳ���������õ��������ʵ����valueReal
{
	int id = rx_message.Data[0]-1;
	u16 p_int = (rx_message.Data[1] << 8) | rx_message.Data[2];		   //���λ��
	u16 v_int = (rx_message.Data[3] << 4) | (rx_message.Data[4] >> 4); //����ٶ�
	u16 t_int = (rx_message.Data[4] & 0x0f) << 8 | rx_message.Data[5]; //���Ť��

	float p = uint2float(p_int, P_MIN, P_MAX, 16);
	float v = uint2float(v_int, V_MIN, V_MAX, 12);
	Tmotor[id].valueReal.Preanlge=Tmotor[id].valueReal.angle;
	Tmotor[id].valueReal.angle = (float)(p / Tmotor[id].param.angle2RAD / Tmotor[id].param.GearRadio / Tmotor[id].param.RATIO);
	Tmotor[id].valueReal.speed = (float)(v / Tmotor[id].param.angle2RAD / Tmotor[id].param.GearRadio / Tmotor[id].param.RATIO);
	Tmotor[id].valueReal.torque = uint2float(t_int, T_MIN, T_MAX, 12);
	
}
