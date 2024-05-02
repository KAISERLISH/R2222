#ifndef _QUEUE_H
#define _QUEUE_H
#include "stm32f4xx.h"
#include "stdbool.h"
#include "string.h"
#include "stm32f4xx_can.h"
#include "stdint.h"

#include "param.h"
#include "led.h"
#include "controllist.h"

/*-----------------------宏定义区--------------------*/
#define CAN_HAVESENT_QUEUESIZE 8
#define CAN_QUEUESIZE 50 // 报文队列长度

#define CAN1_NodeNumber 6u // 控制表节点数目
#define CAN2_NodeNumber 6u

#define IncCAN1Rear ((CAN1_SendQueue.Rear + 1) % CAN_QUEUESIZE)
#define IncCAN2Rear ((CAN2_SendQueue.Rear + 1) % CAN_QUEUESIZE)
#define IncVESCRear ((VESC_SendQueue.Rear + 1) % CAN_QUEUESIZE)
#define IncZDriveRear ((ZDrive_SendQueue.Rear + 1) % CAN_QUEUESIZE)
#define IncDMRear ((DM_SendQueue.Rear + 1) % CAN_QUEUESIZE)

/*--------------------------------------------------*/
/*------------------------类型名区-------------------*/
typedef struct
{
    uint32_t ID; // 报文ID号
    uint8_t DLC; // 数据帧报文数据字节长度
    uint8_t Data[8];
    bool InConGrpFlag;    // 是否进入报文控制表标志位
} CAN_DataStructSendType; // CAN发送报文结构体类型

typedef struct
{
    uint8_t Front, Rear;
    uint8_t IDE;
    CAN_DataStructSendType CAN_DataSend[CAN_QUEUESIZE];
} CAN_SendQueueType;

typedef struct
{
    int32_t SendNumber;
    int32_t ReceiveNumber;
    uint32_t QueueFullTimeout;
    uint8_t TimeOut;
    int32_t SendSem;
    CAN_SendQueueType SendQueue;
} MesgControlGrpType; // 　报文控制表类型

/*---------------------------------------------------*/
/*--------------function declaration area------------*/

void CAN_InitSendQueue(void);
bool CAN_Queue_ifEmpty(CAN_SendQueueType *can_queue);
bool CAN_Queue_ifFull(CAN_SendQueueType *can_queue);

void CAN_QueueOut(CAN_TypeDef *CANx, CAN_SendQueueType *can_queue);

void InitMesgControlList(MesgControlGrpType *CAN_MesgControlList, CAN_TypeDef *CANx);
void MesgConstrolList(MesgControlGrpType *ControlList, CAN_SendQueueType *can_queue, CAN_TypeDef *CANx);

extern MesgControlGrpType CAN1_MesgControlList[CAN1_NodeNumber], CAN2_MesgControlList[CAN2_NodeNumber]; // 报文控制表

extern CAN_SendQueueType CAN1_SendQueue, CAN2_SendQueue, VESC_SendQueue, ZDrive_SendQueue,DM_SendQueue;               // CAN口报文发送队列  其中VESC是通过CAN2发送的
extern MesgControlGrpType CAN1_MesgControlList[CAN1_NodeNumber], CAN2_MesgControlList[CAN2_NodeNumber]; // 报文控制表

#endif
