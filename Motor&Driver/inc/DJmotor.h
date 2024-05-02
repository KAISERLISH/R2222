#ifndef _DJMOTOR_H
#define _DJMOTOR_H
#include "stm32f4xx.h"
#include "stdbool.h"
#include "stm32f4xx_can.h"
#include "includes.h"

#include "pid.h"
#include "param.h"
#include "mathFunc.h"
#include "beep.h"
#include "led.h"
#include "queue.h"
#include "mech.h"

#define DJ_MOTOR1_RX 0x201
#define DJ_MOTOR2_RX 0x202 
#define DJ_MOTOR3_RX 0x203
#define DJ_MOTOR4_RX 0x204
#define DJ_MOTOR5_RX 0x205		
#define DJ_MOTOR6_RX 0x206		
#define DJ_MOTOR7_RX 0x207
#define DJ_MOTOR8_RX 0x208

enum DJ_Motor_Mode{DJRPM,DJPOSITION,DJZERO};// 枚举定义 速度/位置/寻零模式

//加了 voliatile，就在需要用的时候，程序重新去那个地址去提取，保证是最新的             vs16 即 __IO int16_t，也就是 voliatile int16_t
//电机状态值结构体
typedef struct
{
  volatile float angle;// 输出轴角度
	vs16 current;        // 电流
	vs16 speed_read;
	vs16 speed;          //速度
	vs32 pulse;	         //累计脉冲
	vs16 pulseRead;	      //编码器反馈脉冲
	vs16 temperature;
	float current_Read;
}DJMotorValueTypedef;



/**
 * @brief 电机结构参数
 * 
 */
typedef struct 
{
  uint16_t PulsePerRound;	//编码器线数
	uint8_t RATIO;	//电机自带减速比
	int16_t Current_Limit;// 电流限制
	float GearRadio;	//机构齿数比
}DJMotorParamTypedef;


// 电机目前状态
typedef struct
{
    bool arrived;
    bool struck;//堵转
    bool timeout;// 超时
    bool isSetZero; //归零
}DJMotorStatusTypedef;

// 电机限制
typedef struct 
{
    bool PosLimit_ON;
	// M2006是转向电机故设得有转角幅度限制 但在.c文件中好像没开启这个限制
    float maxAngle;// 位置模式的最大转角 
    float minAngle;// 位置模式的最小转角 

    bool PosSPLimit_ON;
    uint16_t PosSPLimit;// 位置模式的速度限制

    bool isReleaseWhenStuck;
    bool stuckDetection_ON;
    bool timeoutDetection_ON;

    int16_t ZeroSP;// 寻零模式下的速度
    uint16_t ZeroCurrent;// 寻零电流
}DJMotorLimitTypedef;

typedef struct 
{
    volatile int32_t lockPulse;//锁位置脉冲
    int16_t distance;  //当前反馈脉冲与上次反馈脉冲之差
	u32 lastRxTime;
    u16 ZeroCnt;	
	u16 TimeoutCnt;		//范围是从0-65535，超过后会重新从0开始计数
	u16 StuckCnt;
}DJMotorArgumTypedef;

typedef struct{
	u32 ReceCnt;
	volatile bool enable;
	volatile bool begin;
	unsigned char mode;
	DJMotorValueTypedef valueSet,valueReal,valuePrv;
	DJMotorParamTypedef Param;
	DJMotorStatusTypedef Status;
	DJMotorLimitTypedef Limit;
	DJMotorArgumTypedef argum;
	Inc_PIDTypedef PID_POS,PID_RPM;
} DJMotor;

extern DJMotor DJmotor[3];

void DJmotor_Init(void);
void DJ_SetZero(uint8_t id);
void DJ_ZeroMode(u8 id);
void DJ_Position_Calculate(uint8_t id);
void DJ_PositionMode(uint8_t id);
void DJ_LockPosition(uint8_t id);
void DJFunction(void);
void DJ_ifTimeOut(uint8_t id);
void DJ_Feedback_Stuck(uint8_t id, uint8_t InConGrpFlag);
void DJ_ReceiveData_CAN2(CanRxMsg Rx_message0);

#endif
