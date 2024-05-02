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

//电机极限参数（见电机说明）
#define P_MIN -12.5f    // 位置极限（rad）
#define P_MAX 12.5f        
#define V_MIN -45.0f    // 速度极限（Rad/s）
#define V_MAX 45.0f
#define KP_MIN 0.0f     // KP极限（N-m/rad）
#define KP_MAX 500.0f
#define KD_MIN 0.0f     // KD极限（N-m/rad/s）
#define KD_MAX 5.0f
#define T_MIN -15.0f		//扭矩极限（Nm）
#define T_MAX 15.0f 

enum TmotorMode
{
	TPos,		//位置模式-0
	TVel,		//速度模式-1
	TPosVel	//速度位置模式-2
};

typedef struct
{
	float angle2RAD;
	float RATIO;
	float GearRadio;
} TmotorParamTypedef;

typedef struct
{
	float angle;	//角度
	float speed;	//速度
	float torque;	//扭矩
	float Preanlge;
} TmotorValueRealTypedef;

typedef struct
{
	float angle;	//角度
	float speed;	//速度
	float torque;	//扭矩
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
