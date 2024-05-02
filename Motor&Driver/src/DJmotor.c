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

    M2006Param.PulsePerRound = 8192; //编码器线数
    M2006Param.RATIO = 36;           //电机自带减速比
    M2006Param.Current_Limit = 4500; //-10000 ~ 10000 - -10A~10A
    M2006Param.GearRadio = 1.0f;     //机构齿数比

    MotorLimit.PosSPLimit_ON = true;
    MotorLimit.PosSPLimit = 3000; //轴前  //TODO:这里暂时把速度提高一下
    MotorLimit.maxAngle = 270;
    MotorLimit.minAngle = -270;

    MotorLimit.ZeroSP = 400;
    MotorLimit.ZeroCurrent = 2000;

    MotorLimit.stuckDetection_ON = false;
    MotorLimit.timeoutDetection_ON = true;
    MotorLimit.isReleaseWhenStuck = false; // 堵转的时候是否释放

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
 * @brief 寻找零点位置  累计脉冲置零 角度置零 锁定脉冲置零
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
 * @brief 寻零模式 以规定的寻零速度寻零
 * 累计脉冲在连续小于10达到100次后认为已经完成
 *
 */
void DJ_ZeroMode(uint8_t id)
{
    DJmotor[id].PID_RPM.SetVal = DJmotor[id].Limit.ZeroSP; // 以预先设定的速度500回零
    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed;

    Inc_PID_Operation(&DJmotor[id].PID_RPM);

    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta;

    PEAK(DJmotor[id].valueSet.current, DJmotor[id].Limit.ZeroCurrent); // 限制回零电流

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
 * @brief 计算累计脉冲和目前角度以及锁点位置
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

    if (DJmotor[id].begin)                                         //锁点位置纪录
        DJmotor[id].argum.lockPulse = DJmotor[id].valueReal.pulse; //累计脉冲
    if (DJmotor[id].Status.isSetZero)
    {
        DJ_SetZero(id);
    }
}

/**
 * @brief 根据累计脉冲当前锁定位置
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
    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta; // pid计算出电流
		PEAK(DJmotor[id].valueSet.current , 3000);
}

/**
 * @brief 位置模式 位置环的增量式pid调节计算值作为速度环的输入，
 * 速度环的增量式计算输出值作为电流环的输入（电流给定）
 */
void DJ_PositionMode(uint8_t id)
{
    DJmotor[id].valueSet.pulse = DJmotor[id].valueSet.angle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360; // 设定的累计脉冲
    DJmotor[id].PID_POS.SetVal = DJmotor[id].valueSet.pulse;                                                                                                 // 设定值赋值给位置Pid结构体

    if (DJmotor[id].Limit.PosLimit_ON) // 限制开关
    {
        if (DJmotor[id].PID_POS.SetVal > DJmotor[id].Limit.maxAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360) // 限制最大
            DJmotor[id].PID_POS.SetVal = DJmotor[id].Limit.maxAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360;
        if (DJmotor[id].PID_POS.SetVal < DJmotor[id].Limit.minAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360) // 限制最小
            DJmotor[id].PID_POS.SetVal = DJmotor[id].Limit.minAngle * DJmotor[id].Param.PulsePerRound * DJmotor[id].Param.RATIO * DJmotor[id].Param.GearRadio / 360;
    }

    DJmotor[id].PID_POS.CurVal = DJmotor[id].valueReal.pulse;

    Inc_PID_Operation(&DJmotor[id].PID_POS); // 位置环的增量式pid调节计算

    DJmotor[id].PID_RPM.SetVal = DJmotor[id].PID_POS.delta; // 速度设定 速度环的输入是位置环PID调节后的输出

    if (DJmotor[id].Limit.PosSPLimit_ON) // 初始化时true
        PEAK(DJmotor[id].PID_RPM.SetVal, DJmotor[id].Limit.PosSPLimit);

    DJmotor[id].PID_RPM.CurVal = DJmotor[id].valueReal.speed; // 读取当前速度值

    Inc_PID_Operation(&DJmotor[id].PID_RPM); //速度环增量式PID调节

    DJmotor[id].valueSet.current += DJmotor[id].PID_RPM.delta; // 电流给定是速度环PID调节后的输出（有累计）

    //以下两句没用到
    if (ABS(DJmotor[id].valueSet.pulse - DJmotor[id].valueReal.pulse) < 60)
        DJmotor[id].Status.arrived = true;
    else
        DJmotor[id].Status.arrived = false;
}

/**
 * @brief 单一的增量式PID算法 速度环的PID计算输出作为电流环的输入
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
 * @brief 是否超时报警，30*10=300ms 在DJFunction()函数中判断调用，即也在定时中断中
 *
 */
void DJ_ifTimeOut(uint8_t id)
{
    // 每次接受到can报文都会对lasttime置零
    if ((DJmotor[id].enable == true) && ((DJmotor[id].argum.lastRxTime++) > 30))
    {
        DJmotor[id].argum.TimeoutCnt++;
        if (DJmotor[id].argum.TimeoutCnt > 10) // 大于30*10=300Ms时报警
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
        if (DJmotor[id].mode == DJRPM) //速度模式
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

                    if (DJmotor[id].Limit.isReleaseWhenStuck) // 如果堵转后释放
                        DJmotor[id].enable = false;
                }
            }
            else if ((DJmotor[id].enable == true) && (DJmotor[id].Status.struck == true)) // 堵转处理后
            {
                DJmotor[id].Status.struck = false;
                //B1p.error = 0;
            }
            else // 没有堵转
                DJmotor[id].argum.StuckCnt = 0;
        }
        //位置模式
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
 * @brief 发送报文数据
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

    DJtx_message.IDE = CAN_Id_Standard; // 标准帧

    DJtx_message.RTR = CAN_RTR_DATA; // 数据帧

    DJtx_message.DLC = 8;

    uint8_t t = 2 * (id & 0x0B);

    EncodeS16Data(&DJmotor[id].valueSet.current, &DJtx_message.Data[t]);

    ChangeDataByte(&DJtx_message.Data[t], &DJtx_message.Data[t + 1]); // 交换高低位，低8位是控制电流的高8位，高8位是控制电流的低8位
		// TODO:

    CAN_Transmit(CAN2, &DJtx_message);
}

/**
 * @brief 在定时中断中调用，每一毫秒发送一次
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
                DJ_LockPosition(id); // 电机不开始就锁定
        }
        DJ_CurrentTransmit(id);
    }
}

void DJ_ReceiveData_CAN2(CanRxMsg Rx_message0) // 后缀 0 表示从 FIFO0 接收
{
    if (Rx_message0.StdId >= 0x201 && Rx_message0.StdId <= 0x208)
    {
        // DJMOTORpointer motor;
        uint8_t ID = Rx_message0.StdId - 0x200;
        for (int k = 0; k <= 7; k++)
        {
            if (ID == (k+1))
            {
                DJmotor[k].valueReal.pulseRead = (Rx_message0.Data[0] << 8) | Rx_message0.Data[1]; // 小端存储 高位在前低位在后
                DJmotor[k].valueReal.speed = (Rx_message0.Data[2] << 8) | Rx_message0.Data[3];
                DJmotor[k].valueReal.current = (Rx_message0.Data[4] << 8) | Rx_message0.Data[5];
                if (DJmotor[k].Param.RATIO == 19) // M3508
                {
                    DJmotor[k].valueReal.speed = (float)(DJmotor[k].valueReal.speed * 0.0263157);
                    DJmotor[k].valueReal.temperature = Rx_message0.Data[6];
                    DJmotor[k].valueReal.current_Read = (float)(DJmotor[k].valueReal.current * 0.0012207); // 实际上是current / 16384 *20
                }
                else // M2006
                    DJmotor[k].valueReal.current_Read = (float)(DJmotor[k].valueReal.current / 10000 * 10);

                DJ_Position_Calculate(k);

                DJmotor[k].argum.lastRxTime = 0; // 接收到报文即置 0
            }
        }
    }
}
