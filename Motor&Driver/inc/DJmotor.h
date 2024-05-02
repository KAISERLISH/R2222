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

enum DJ_Motor_Mode{DJRPM,DJPOSITION,DJZERO};// ö�ٶ��� �ٶ�/λ��/Ѱ��ģʽ

//���� voliatile��������Ҫ�õ�ʱ�򣬳�������ȥ�Ǹ���ַȥ��ȡ����֤�����µ�             vs16 �� __IO int16_t��Ҳ���� voliatile int16_t
//���״ֵ̬�ṹ��
typedef struct
{
  volatile float angle;// �����Ƕ�
	vs16 current;        // ����
	vs16 speed_read;
	vs16 speed;          //�ٶ�
	vs32 pulse;	         //�ۼ�����
	vs16 pulseRead;	      //��������������
	vs16 temperature;
	float current_Read;
}DJMotorValueTypedef;



/**
 * @brief ����ṹ����
 * 
 */
typedef struct 
{
  uint16_t PulsePerRound;	//����������
	uint8_t RATIO;	//����Դ����ٱ�
	int16_t Current_Limit;// ��������
	float GearRadio;	//����������
}DJMotorParamTypedef;


// ���Ŀǰ״̬
typedef struct
{
    bool arrived;
    bool struck;//��ת
    bool timeout;// ��ʱ
    bool isSetZero; //����
}DJMotorStatusTypedef;

// �������
typedef struct 
{
    bool PosLimit_ON;
	// M2006��ת�����������ת�Ƿ������� ����.c�ļ��к���û�����������
    float maxAngle;// λ��ģʽ�����ת�� 
    float minAngle;// λ��ģʽ����Сת�� 

    bool PosSPLimit_ON;
    uint16_t PosSPLimit;// λ��ģʽ���ٶ�����

    bool isReleaseWhenStuck;
    bool stuckDetection_ON;
    bool timeoutDetection_ON;

    int16_t ZeroSP;// Ѱ��ģʽ�µ��ٶ�
    uint16_t ZeroCurrent;// Ѱ�����
}DJMotorLimitTypedef;

typedef struct 
{
    volatile int32_t lockPulse;//��λ������
    int16_t distance;  //��ǰ�����������ϴη�������֮��
	u32 lastRxTime;
    u16 ZeroCnt;	
	u16 TimeoutCnt;		//��Χ�Ǵ�0-65535������������´�0��ʼ����
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
