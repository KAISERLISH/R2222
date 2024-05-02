/*
 * @Author: qijunhao 2020302191427@whu.edu.cn
 * @Date: 2022-10-03 20:26:14
 * @LastEditors: qijunhao 2020302191427@whu.edu.cn
 * @LastEditTime: 2022-12-07 21:46:12
 * @FilePath: \Chassis_f\User\src\param.c
 * @Description:
 *
 * Copyright (c) 2022 by qijunhao 2020302191427@whu.edu.cn, All Rights Reserved.
 */
#include "param.h"


FlagType Flag;
LEDStructType Led;
BeepStructType Beep;
void param_Init(void)
{
	//	flag.Can1MsgCtrllistEnable=1;
	//	flag.Can2MsgCtrllistEnable=0;

	//Led8DisData(0);
	Led.Normal = 1;
	Beep.AllowFlag = true;
	
	Zdrive_Init();
	DJmotor_Init();
	Tmotor_Init();
	Tmotor_Comm_Init(CAN2,TmotorID);
}
