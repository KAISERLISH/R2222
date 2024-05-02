/**
 * @file queue.c
 * @brief
 *  注意这是机构的队列函数文件
 * @author ZhiHang_FU (fu_zhihang@126.com)
 * @version 1.0
 * @date
 * @copyright Copyright (c) 2023  the author of this program
 */
#include "queue.h"

CAN_SendQueueType CAN1_SendQueue, CAN2_SendQueue, VESC_SendQueue, ZDrive_SendQueue, DM_SendQueue; // CAN口报文发送队列  其中VESC是通过CAN2发送的
MesgControlGrpType CAN1_MesgControlList[CAN1_NodeNumber], CAN2_MesgControlList[CAN2_NodeNumber]; // 报文控制表

/**
 * @brief Initializ CAN message queue
 */
void CAN_InitSendQueue(void)
{
    CAN1_SendQueue.Front = CAN1_SendQueue.Rear = 0;
    CAN2_SendQueue.Front = CAN2_SendQueue.Rear = 0;
    VESC_SendQueue.Front = VESC_SendQueue.Rear = 0;
    ZDrive_SendQueue.Front = ZDrive_SendQueue.Rear = 0;
    DM_SendQueue.Front = DM_SendQueue.Rear = 0;
    VESC_SendQueue.IDE = CAN_ID_EXT;
    ZDrive_SendQueue.IDE = CAN_ID_STD;
    DM_SendQueue.IDE = CAN_ID_STD;
}

/**
 * @brief Judge whether the CAN queue is empty
 * @param  can_queueMy Param doc
 * @return true
 * @return false
 */
bool CAN_Queue_ifEmpty(CAN_SendQueueType *can_queue)
{
    return ((can_queue->Front == can_queue->Rear));
}

/**
 * @brief Judge whether the CAN queue is full
 * @param  can_queueMy Param doc
 * @return true
 * @return false
 */
bool CAN_Queue_ifFull(CAN_SendQueueType *can_queue)
{
    return (((can_queue->Rear + 1) % CAN_QUEUESIZE) == can_queue->Front);
}

/**
 * @brief  Out of CAN queue
 * @param  CANx   select which CAN channel
 * @param  can_queue
 */
void CAN_QueueOut(CAN_TypeDef *CANx, CAN_SendQueueType *can_queue)
{
    CanTxMsg tx_message; // CAN报文发送结构体临时变量

    if (CAN_Queue_ifEmpty(can_queue))
    {
        Flag.CANSendQueueEMPTY = true;
        return;
    }

    if (CANx == CAN1) // 回复主控
    {
        MesgConstrolList(CAN1_MesgControlList, can_queue, CAN1);
        tx_message.IDE = CAN_ID_EXT; // 扩展帧 反馈主控
        tx_message.ExtId = can_queue->CAN_DataSend[can_queue->Front].ID;
    }

    else if (CANx == CAN2)
    {
        tx_message.IDE = can_queue->IDE; // 拓展帧
        if (can_queue->IDE == CAN_ID_STD)
        {
            tx_message.StdId = can_queue->CAN_DataSend[can_queue->Front].ID;
        }
        else if (can_queue->IDE == CAN_ID_EXT)
        {
            if ((can_queue->CAN_DataSend[can_queue->Front].ID & 0xf0000000) == 0xf0000000) // 好像是为了区分VESC
            {
                can_queue->CAN_DataSend[can_queue->Front].ID &= 0x0fffffff;
            }
            tx_message.ExtId = can_queue->CAN_DataSend[can_queue->Front].ID;
        }
        MesgConstrolList(CAN2_MesgControlList, can_queue, CAN2);
        // // can2 对应的是VESC驱动的电机
        //  if ((can_queue->CAN_DataSend[can_queue->Front].ID & 0xf0000000) == 0xf0000000) // 好像是为了区分
        // {
        //     can_queue->CAN_DataSend[can_queue->Front].ID &= 0x0fffffff;
        //     tx_message.IDE = CAN_ID_EXT; // 拓展帧
        //     tx_message.ExtId = can_queue->CAN_DataSend[can_queue->Front].ID;
        // }

        // else
        // {
        //     tx_message.IDE = CAN_ID_STD; // 这个是临时为了zdirve写的
        //     tx_message.StdId = can_queue->CAN_DataSend[can_queue->Front].ID;
        // }
        // MesgConstrolList(CAN2_MesgControlList, can_queue, CAN2);
    }

    tx_message.DLC = can_queue->CAN_DataSend[can_queue->Front].DLC;
    tx_message.RTR = CAN_RTR_Data; // 数据帧
    memcpy(tx_message.Data, can_queue->CAN_DataSend[can_queue->Front].Data, tx_message.DLC * sizeof(uint8_t));

    can_queue->Front = (can_queue->Front + 1) % CAN_QUEUESIZE;

    CAN_Transmit(CANx, &tx_message);
}

