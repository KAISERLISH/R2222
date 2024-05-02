#ifndef _TMOTOR_H
#define _TMOTOR_H

#include "queue.h"
#include "mathFunc.h"
#include "tim3.h"
#include "param.h"
#include "mech.h"
#define TmotorRX 0x00
#define ZDriveRX 0x01

#define TmotorID 1

//������޲����������˵����
#define P_MIN -12.5f    // λ�ü��ޣ�rad��
#define P_MAX 12.5f        
#define V_MIN -45.0f    // �ٶȼ��ޣ�Rad/s��
#define V_MAX 45.0f
#define KP_MIN 0.0f     // KP���ޣ�N-m/rad��
#define KP_MAX 500.0f
#define KD_MIN 0.0f     // KD���ޣ�N-m/rad/s��
#define KD_MAX 5.0f
#define T_MIN -15.0f		//Ť�ؼ��ޣ�Nm��
#define T_MAX 15.0f 

enum TmotorMode
{
	TPos,		//λ��ģʽ-0
	TVel,		//�ٶ�ģʽ-1
	TPosVel	//�ٶ�λ��ģʽ-2
};

typedef struct
{
	float angle2RAD;
	float RATIO;
	float GearRadio;
} TmotorParamTypedef;

typedef struct
{
	float angle;	//�Ƕ�
	float speed;	//�ٶ�
	float torque;	//Ť��
	float Preanlge;
} TmotorValueRealTypedef;

typedef struct
{
	float angle;	//�Ƕ�
	float speed;	//�ٶ�
	float torque;	//Ť��
	float kp;
	float kd;
}TmotorValueSetTypedef;


typedef struct
{
	bool timeout;
	bool stuck;
} TmotorStatusTypedef;

typedef struct
{
	bool enable;
	bool SetZero;
	enum TmotorMode mode;
	TmotorValueRealTypedef valueReal;
	TmotorValueSetTypedef valueSet;
	TmotorParamTypedef param;
	TmotorStatusTypedef status;
} TMotorTypedef;

u16 float2uint(float x, float x_min, float x_max, u8 bits);
float uint2float(int x_int, float x_min, float x_max, int bits);

void Tmotor_Init(void);
void Tmotor_Comm_Init(CAN_TypeDef *CANx, u8 ID);
void Tmotor_Control(u8 ID, float f_pos, float f_vel, float f_kp, float f_kd, float f_torque);
void TmotorPVMode(u8 ID);
void Tmotor_SetZero(u8 ID);
void TmotorFunc(void);
void TmotorReceiveHandler(CanRxMsg rx_message);

extern TMotorTypedef Tmotor[1];
#endif
