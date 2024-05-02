#include "DJmotor.h"

DJMotor DJmotor[3];

DJMotorParamTypedef M2006Param, M3508Param;
DJMotorStatusTypedef Status;
DJMotorLimitTypedef MotorLimit;
DJMotorArgumTypedef MotorArgum;
Inc_PIDTypedef PID_POS, PID_RPM;

void DJmotor_Init(void)
{
    M3508Param.PulsePerRound = 8192;
    M3508Param.RATIO = 19;
    M3508Param.Current_Limit = 5000; //-16384 ~ 16384 - -20A ~20A
    M3508Param.GearRadio = 1.0;

    M2006Param.PulsePerRound = 8192; //����������
    M2006Param.RATIO = 36;           //����Դ����ٱ�
    M2006Param.Current_Limit = 4500; //-10000 ~ 10000 - -10A~10A
    M2006Param.GearRadio = 1.0f;     //����������

    MotorLimit.PosSPLimit_ON = true;
    MotorLimit.PosSPLimit = 3000; //��ǰ  //TODO:������ʱ���ٶ����һ��
    MotorLimit.maxAngle = 270;
    MotorLimit.minAngle = -270;

    MotorLimit.ZeroSP = 400;
    MotorLimit.ZeroCurrent = 2000;

    MotorLimit.stuckDetection_ON = false;
    MotorLimit.timeoutDetection_ON = true;
    MotorLimit.isReleaseWhenStuck = false; // ��ת��ʱ���Ƿ��ͷ�

    MotorArgum.lockPulse = 0;

    for (short i = 0; i <= 1; i++)
    {
        DJmotor[i].Limit = MotorLimit;
        DJmotor[i].argum = MotorArgum;
        DJmotor[i].Param = M2006Param;

        DJmotor[i].enable = false;
        DJmotor[i].begin = false;
        DJmotor[i].mode = DJPOSITION;
        DJmotor[i].valueSet.angle = 0;
        DJmotor[i].valueSet.speed = 100;
        DJmotor[i].valueSet.current = 0;
        Inc_PID_Init(&DJmotor[i].PID_POS, 3, 0.06, 0.0, DJmotor[i].valueSet.pulse);
        Inc_PID_Init(&DJmotor[i].PID_RPM, 8.0, 0.5, 0.2, DJmotor[i].valueSet.speed);
    }
		for (short i = 2; i <= 3; i++)
    {
        DJmotor[i].Limit = MotorLimit;
		DJmotor[i].Limit.ZeroCurrent = MotorLimit.ZeroCurrent=4000;
        DJmotor[i].argum = MotorArgum;
        DJmotor[i].Param = M3508Param;

        DJmotor[i].enable = false;
        DJmotor[i].begin = false;
        DJmotor[i].mode = DJPOSITION;
        DJmotor[i].valueSet.angle = 0;
        DJmotor[i].valueSet.speed = 100;
        DJmotor[i].valueSet.current = 0;
        Inc_PID_Init(&DJmotor[i].PID_POS, 7, 0.03, 0.0, DJmotor[i].valueSet.pulse);
        Inc_PID_Init(&DJmotor[i].PID_RPM, 8, 0.5, 0.02, DJmotor[i].valueSet.speed);
    }
		DJmotor[DJ3508UD].Limit.ZeroSP=-2000;
		DJmotor[DJ2006LR].Limit.ZeroSP=100;
		DJmotor[DJ2006LR].Limit.PosSPLimit=6000;
			
}

/**
 * @brief Ѱ�����λ��  �ۼ��������� �Ƕ����� ������������
 *
 */
void DJ_SetZero(uint8_t id)
{
    DJmotor[id].Status.isSetZero = false;
    DJmotor[id].valueReal.pulse = 0;
    DJmotor[id].valueReal.angle = 0;
    DJmotor[id].argum.lockPulse = 0;
}

/**
 * @brief Ѱ��ģʽ �Թ涨��Ѱ���ٶ�Ѱ��
 * �ۼ�����������С��10�ﵽ100�κ���Ϊ�Ѿ����
 *
 */
void DJ_ZeroMode(uint8_t id)
{
    DJmotor[id].PID_RPM.SetVal = DJmotor[id].Limit.ZeroSP; // ��Ԥ���趨���ٶ�500����
    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed;

    Inc_PID_Operation(&DJmotor[id].PID_RPM);

    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta;

    PEAK(DJmotor[id].valueSet.current, DJmotor[id].Limit.ZeroCurrent); // ���ƻ������

    if (ABS(DJmotor[id].argum.distance) < 10)
        DJmotor[id].argum.ZeroCnt++;
    else
        DJmotor[id].argum.ZeroCnt = 0;

    if (DJmotor[id].argum.ZeroCnt > 150)
    {
       
				DJmotor[id].argum.ZeroCnt = 0;
        DJ_SetZero(id);
				beep_show(1);
				DJmotor[id].mode=DJPOSITION;
        DJmotor[id].begin = false;
    }
}

/**
 * @brief �����ۼ������Ŀǰ�Ƕ��Լ�����λ��
 *
 */
void DJ_Position_Calculate(uint8_t id)
{
    DJmotor[id].argum.distance = DJmotor[id].valueReal.pulseRead - DJmotor[id].valuePrv.pulseRead;
    DJmotor[id].valuePrv = DJmotor[id].valueReal;

    if (ABS(DJmotor[id].argum.distance) > 4096)
        DJmotor[id].argum.distance = DJmotor[id].argum.distance - PlusOrMinus(DJmotor[id].argum.distance) * DJmotor[id].Param.PulsePerRound;

    DJmotor[id].valueReal.pulse = DJmotor[id].valueReal.pulse + DJmotor[id].argum.distance;
    DJmotor[id].valueReal.angle = DJmotor[id].valueReal.pulse * 360.0 / DJmotor[id].Param.PulsePerRound / DJmotor[id].Param.RATIO / DJmotor[id].Param.GearRadio;

    if (DJmotor[id].begin)                                         //����λ�ü�¼
        DJmotor[id].argum.lockPulse = DJmotor[id].valueReal.pulse; //�ۼ�����
    if (DJmotor[id].Status.isSetZero)
    {
        DJ_SetZero(id);
    }
}

/**
 * @brief �����ۼ����嵱ǰ����λ��
 *
 */
void DJ_LockPosition(uint8_t id)
{
    DJmotor[id].PID_POS.SetVal = DJmotor[id].argum.lockPulse;
    DJmotor[id].PID_POS.CurVal = DJmotor[id].valueReal.pulse;
    Inc_PID_Operation(&DJmotor[id].PID_POS);

    DJmotor[id].PID_RPM.SetVal = DJmotor[id].PID_POS.delta;
    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed;
    Inc_PID_Operation(&DJmotor[id].PID_RPM);
    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta; // pid���������
		PEAK(DJmotor[id].valueSet.current , 3000);
}

/**
 * @brief λ��ģʽ λ�û�������ʽpid���ڼ���ֵ��Ϊ�ٶȻ������룬
 * �ٶȻ�������ʽ�������ֵ��Ϊ�����������루����������
 */
void DJ_PositionMode(uint8_t id)
{
    DJmotor[id].valueSet.pulse = DJmotor[id].valueSet.angle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360; // �趨���ۼ�����
    DJmotor[id].PID_POS.SetVal = DJmotor[id].valueSet.pulse;                                                                                                 // �趨ֵ��ֵ��λ��Pid�ṹ��

    if (DJmotor[id].Limit.PosLimit_ON) // ���ƿ���
    {
        if (DJmotor[id].PID_POS.SetVal > DJmotor[id].Limit.maxAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360) // �������
            DJmotor[id].PID_POS.SetVal = DJmotor[id].Limit.maxAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360;
        if (DJmotor[id].PID_POS.SetVal < DJmotor[id].Limit.minAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360) // ������С
            DJmotor[id].PID_POS.SetVal = DJmotor[id].Limit.minAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360;
    }

    DJmotor[id].PID_POS.CurVal = DJmotor[id].valueReal.pulse;

    Inc_PID_Operation(&DJmotor[id].PID_POS); // λ�û�������ʽpid���ڼ���

    DJmotor[id].PID_RPM.SetVal = DJmotor[id].PID_POS.delta; // �ٶ��趨 �ٶȻ���������λ�û�PID���ں�����

    if (DJmotor[id].Limit.PosSPLimit_ON) // ��ʼ��ʱtrue
        PEAK(DJmotor[id].PID_RPM.SetVal, DJmotor[id].Limit.PosSPLimit);

    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed; // ��ȡ��ǰ�ٶ�ֵ

    Inc_PID_Operation(&DJmotor[id].PID_RPM); //�ٶȻ�����ʽPID����

    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta; // �����������ٶȻ�PID���ں����������ۼƣ�

    //��������û�õ�
    if (ABS(DJmotor[id].valueSet.pulse - DJmotor[id].valueReal.pulse) < 60)
        DJmotor[id].Status.arrived = true;
    else
        DJmotor[id].Status.arrived = false;
}

/**
 * @brief ��һ������ʽPID�㷨 �ٶȻ���PID���������Ϊ������������
 *
 */
void DJ_SpeedMode(uint8_t id)
{
    DJmotor[id].PID_RPM.SetVal = DJmotor[id].valueSet.speed;
    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed;
    Inc_PID_Operation(&DJmotor[id].PID_RPM);

    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta;
}

/**
 * @brief �Ƿ�ʱ������30*10=300ms ��DJFunction()�������жϵ��ã���Ҳ�ڶ�ʱ�ж���
 *
 */
void DJ_ifTimeOut(uint8_t id)
{
    // ÿ�ν��ܵ�can���Ķ����lasttime����
    if ((DJmotor[id].enable == true) && ((DJmotor[id].argum.lastRxTime++) > 30))
    {
        DJmotor[id].argum.TimeoutCnt++;
        if (DJmotor[id].argum.TimeoutCnt > 10) // ����30*10=300Msʱ����
        {
            DJmotor[id].argum.TimeoutCnt = 0;

            DJmotor[id].Status.timeout = true;
//            LedTask.Timeout = true;
            // B1p.error=1;
            //Led8DisData(id + 1);
        }
        else
        {
            DJmotor[id].Status.timeout = 0;
        }
    }

    else if ((DJmotor[id].enable == true) && (DJmotor[id].Status.timeout))
    {
        DJmotor[id].Status.timeout = 0;
        //B1p.error = 0;
    }
    else
        DJmotor[id].argum.TimeoutCnt = 0;
}

void DJ_ifStuck(uint8_t id)
{
    if (DJmotor[id].enable == true)
    {
        if (DJmotor[id].mode == DJRPM) //�ٶ�ģʽ
        {
            if (ABS(DJmotor[id].argum.distance) < 5)
            {
                DJmotor[id].argum.StuckCnt++;
                if (DJmotor[id].argum.StuckCnt > 3000)
                {
                    DJmotor[id].Status.struck = true;
                    DJmotor[id].argum.StuckCnt = 0;
                    DJ_Feedback_Stuck(id, 0);
                    //LedTask.Stuck = 1;
                    //B1p.error = 1;
                    //Led8DisData(id + 1);

                    if (DJmotor[id].Limit.isReleaseWhenStuck) // �����ת���ͷ�
                        DJmotor[id].enable = false;
                }
            }
            else if ((DJmotor[id].enable == true) && (DJmotor[id].Status.struck == true)) // ��ת�����
            {
                DJmotor[id].Status.struck = false;
                //B1p.error = 0;
            }
            else // û�ж�ת
                DJmotor[id].argum.StuckCnt = 0;
        }
        //λ��ģʽ
        else if (DJmotor[id].mode == DJPOSITION)
        {
            if ((ABS(DJmotor[id].valueSet.angle - DJmotor[id].valueReal.angle) > 1) && (ABS(DJmotor[id].argum.distance) < 5))
            {
                DJmotor[id].argum.StuckCnt++;
                if (DJmotor[id].argum.StuckCnt > 3000)
                {
                    DJmotor[id].Status.struck = true;
                    DJmotor[id].argum.StuckCnt = 0;
                    DJ_Feedback_Stuck(id, 0);
                    //LedTask.Stuck = 1;
                    //B1p.error = 1;
                    //Led8DisData(id + 1);

                    if (DJmotor[id].Limit.isReleaseWhenStuck)
                        DJmotor[id].enable = false;
                }
                else if ((DJmotor[id].enable == true) && (DJmotor[id].Status.struck == true))
                {
                    DJmotor[id].Status.struck = false;
                    //B1p.error = 0;
                }
            }
            else
                DJmotor[id].argum.StuckCnt = 0;
        }
    }
}

void DJ_Feedback_Stuck(uint8_t id, uint8_t InConGrpFlag)
{
    if (IncCAN1Rear == CAN1_SendQueue.Front)
    {
        Flag.CAN1SendQueueFULL++;
        return;
    }
    else
    {
			  CAN1_SendQueue.IDE =CAN_ID_STD;
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].ID = 0x285 + id;
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].DLC = 0x06;
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[0] = 'E';
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[1] = 'R';
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[2] = 'R';
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[3] = 0;
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[4] = 'D';
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].Data[5] = 'Z';
        CAN1_SendQueue.CAN_DataSend[CAN1_SendQueue.Rear].InConGrpFlag = InConGrpFlag;
    }
    CAN1_SendQueue.Rear = IncCAN1Rear;
}

/**
 * @brief ���ͱ�������
 *
 */
void DJ_CurrentTransmit(uint8_t id)
{
    static CanTxMsg DJtx_message;
    PEAK(DJmotor[id].valueSet.current, DJmotor[id].Param.Current_Limit);

    if ( (DJmotor[id].enable) == false)
    {
        DJmotor[id].valueSet.current = 0;
    }
    if (id < 4)
        DJtx_message.StdId = 0x200;
    else
        DJtx_message.StdId = 0x1FF;

    DJtx_message.IDE = CAN_Id_Standard; // ��׼֡

    DJtx_message.RTR = CAN_RTR_DATA; // ����֡

    DJtx_message.DLC = 8;

    uint8_t t = 2 * (id & 0x0B);

    EncodeS16Data(&DJmotor[id].valueSet.current, &DJtx_message.Data[t]);

    ChangeDataByte(&DJtx_message.Data[t], &DJtx_message.Data[t + 1]); // �����ߵ�λ����8λ�ǿ��Ƶ����ĸ�8λ����8λ�ǿ��Ƶ����ĵ�8λ
		// TODO:

    CAN_Transmit(CAN2, &DJtx_message);
}

/**
 * @brief �ڶ�ʱ�ж��е��ã�ÿһ���뷢��һ��
 *
 */
void DJFunction(void)
{
    for (int id = 0; id < 4; id++)
    {
        if (DJmotor[id].enable)
        {
            if (DJmotor[id].Limit.timeoutDetection_ON)
            {
                DJ_ifTimeOut(id);
            }
            if (DJmotor[id].begin)
            {
                switch (DJmotor[id].mode)
                {
                case DJRPM:
                    DJ_SpeedMode(id);
                    break;
                case DJPOSITION:
                    DJ_PositionMode(id);
                    break;
                case DJZERO:
                    DJ_ZeroMode(id);
                    break;

                default:
                    break;
                }
            }
            else
                DJ_LockPosition(id); // �������ʼ������
        }
        DJ_CurrentTransmit(id);
    }
}

void DJ_ReceiveData_CAN2(CanRxMsg Rx_message0) // ��׺ 0 ��ʾ�� FIFO0 ����
{
    if (Rx_message0.StdId >= 0x201 && Rx_message0.StdId <= 0x208)
    {
        // DJMOTORpointer motor;
        uint8_t ID = Rx_message0.StdId - 0x200;
        for (int k = 0; k <= 7; k++)
        {
            if (ID == (k+1))
            {
                DJmotor[k].valueReal.pulseRead = (Rx_message0.Data[0] << 8) | Rx_message0.Data[1]; // С�˴洢 ��λ��ǰ��λ�ں�
                DJmotor[k].valueReal.speed = (Rx_message0.Data[2] << 8) | Rx_message0.Data[3];
                DJmotor[k].valueReal.current = (Rx_message0.Data[4] << 8) | Rx_message0.Data[5];
                if (DJmotor[k].Param.RATIO == 19) // M3508
                {
                    DJmotor[k].valueReal.speed = (float)(DJmotor[k].valueReal.speed * 0.0263157);
                    DJmotor[k].valueReal.temperature = Rx_message0.Data[6];
                    DJmotor[k].valueReal.current_Read = (float)(DJmotor[k].valueReal.current * 0.0012207); // ʵ������current / 16384 *20
                }
                else // M2006
                    DJmotor[k].valueReal.current_Read = (float)(DJmotor[k].valueReal.current / 10000 * 10);

                DJ_Position_Calculate(k);

                DJmotor[k].argum.lastRxTime = 0; // ���յ����ļ��� 0
            }
        }
    }
}
