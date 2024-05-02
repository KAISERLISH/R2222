#ifndef _ZDRIVE_H
#define _ZDRIVE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "stm32f4xx_can.h"
#include "includes.h"
#include "queue.h"
#include "pid.h"
#include "param.h"
#include "mathFunc.h"

typedef enum
{
    // mod,0失能,1电流模式,2速度模式,3位置模式,4测试模式,5电阻电感校准,6编码器线性补偿,7编码器偏移校准,8VK校准,9保存配置,10擦除配置,11清除错误,12刹车
    Zdrive_Disable = 0,
    Zdrive_Current,
    Zdrive_Speed,
    Zdrive_Position,
    Zdrive_Test,
    Zdrive_RVCalibration,
    Zdrive_EncoderLineCalibration,
    Zdrive_EncoudeOffsetCalibration,
    Zdrive_VKCalibration,
    Zdrive_SaveSetting,
    Zdrive_EraseSetting,
    Zdrive_ClearErr, // 擦除错误
    Zdrive_Brake
} Zdrive_Mode;
typedef enum
{
    // err,0无,1低电压,2过电压,3电流不稳,4过电流,5超速,6电阻过大,7电感过大,8编码器错误,9极对数不匹配,10KV校准失败,11模式不合法,12参数错误,13高温
    Zdrive_Well,
    Zdrive_InsufficientVoltage,
    Zdrive_OverVoltage,
    Zdrive_InstabilityCurrent,
    Zdrive_OverCurrent,
    Zdrive_OverSpeed,
    Zdrive_ExcessiveR,
    Zdrive_ExcessiveInductence,
    Zdrive_LoseEncoder1,
    Zdrive_PolesErr,
    Zdrive_VKCalibrationErr,
    Zdrive_ModeErr,      // 模式不合法
    Zdrive_ParameterErr, // 参数错误
    Zdrive_Hot
} Zdrive_Err;
typedef struct
{
    float current;
    float speed;
    float angle; 
} ZdriveValueTypedef;

typedef struct
{
    bool timeout;
    bool stuck;
    Zdrive_Err err;
} ZdriveStatusTypedef;
typedef struct
{
    uint32_t lastRxTim;
    uint32_t timeoutTicks;
    uint32_t stuckcnt;
		float lockAngle;
		float vlimit;
} ZdriveArgumTypedef;

typedef struct
{
    float GearRadio;
    float ReductionRatio;
} ZdriveParamTypedef;

typedef struct
{
		bool PVTModeFlag;
		u8 PVTStage,PVTArrivedCnt;
		float ReachTime[3];
		float TargetSpeed[3];
		float TargetAngle[3];
} ZdrivePVTModeTypedef;

typedef struct
{
	float p_p;
	float p_i;
	float v_p;
	float v_i;
}ZdrivePIDTypedef;

typedef struct
{
    bool enable;
    bool begin;
    bool Brake;
    bool errClearFlag;
    Zdrive_Mode mode;
    Zdrive_Mode modeRead;
    ZdriveParamTypedef Param;
		ZdrivePIDTypedef PID,PIDRead;
    ZdriveValueTypedef ValueSet,ValueReal;
    ZdriveStatusTypedef Status;
    ZdriveArgumTypedef Argum;
		ZdrivePVTModeTypedef PVTMode;
} ZdriveTypedef;

extern ZdriveTypedef Zdrive[4]; // pay attention when id=2(Zdrive[1])

void Zdrive_Init(void);

void Zdrive_SetMode(float mode, u8 id);
void Zdrive_SetSpeed(float speed, u8 id);
void Zdrive_SetPosition(float position, u8 id);
void Zdrive_SetAcceleration(float acceleration,u8 id);
void Zdrive_SetDeceleration(float deceleration,u8 id);
void Zdrive_Set_p_PID(float p_p,float p_i,u8 id);
void Zdrive_Set_v_PID(float v_p,float v_i,u8 id);
void Zdrive_SetZero(u8 id);//位置置零
void Zdrive_SetVLimit(float vlimit,u8 id);


void Zdrive_AskErr(u8 id);
void Zdrive_AskSpeed(u8 id);
void Zdrive_AskPosition(u8 id);
void Zdrive_AskVoltage(u8 id);
void Zdrive_AskMode(u8 id);
void Zdrive_Ask_p_PID(u8 id);
void Zdrive_Ask_v_PID(u8 id);
void Zdrive_AskVLimit(u8 id);

void ZdriveFunction(void);
void ZdriveReceiveHandler(CanRxMsg rx_message);

#endif
