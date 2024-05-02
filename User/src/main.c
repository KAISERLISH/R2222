#include "main.h"

int main(void)
{
	SystemInit();
  	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);	// 7/1
	
	CAN1_Init();
	CAN2_Init();
	Tim2_Init();
	Tim3_Init();
	
	LED_Init();
//	Beep_Configuration();
	Valve_Init();
	param_Init();

  	OSInit();	//初始化操作系统
	OSTaskCreate(Task_Start,(void *)0,&START_TASK_STK[TASK_STK_SIZE-1],START_TASK_PRIO);
	OSStart();	//启动操作系统
}

/**
  * @brief  起始任务
  */
static void Task_Start(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	OS_CPU_SysTickInit();		//启动操作系统时钟
	
	OS_ENTER_CRITICAL();
	
	
/**************创建任务*****************/	

	OSTaskCreate(Get_TASK, (void *)0, &Get_TASK_STK[TASK_STK_SIZE - 1], Get_Task_PRIO);
	OSTaskCreate(Identify_TASK, (void *)0, &Identify_TASK_STK[TASK_STK_SIZE-1],Identify_Task_PRIO);	
	OSTaskCreate(Put_TASK, (void *)0, &Put_TASK_STK[TASK_STK_SIZE - 1], Put_Task_PRIO);\
	OSTaskCreate(Ctr_Task,(void *)0, &Ctr_TASK_STK[TASK_STK_SIZE-1],Ctr_Task_PRIO);
	
	OSTaskCreate(Task_LED, (void *)0, &LED_TASK_STK[TASK_STK_SIZE - 1],LED_TASK_PRIO);	
	OSTaskCreate(Task_BEEP, (void *)0, &BEEP_TASK_STK[TASK_STK_SIZE - 1], BEEP_TASK_PRIO);

	OS_EXIT_CRITICAL();		//退出临界区

	OSTimeDly(1000);
	

	for (;;)
	{
		OSTimeDly(2000);
	}
	
}

bool ifGet=false;
u8 ifIdentify=0;
bool ifPut=false;
bool ifAllFindZero=false;
bool ifEnable=false;
bool ifFindZero=false;
bool ifAllSetZero=true;
enum BallType{Nothing,BadBall,GoodBall};
enum BallType Balltype;
int Tmotor_stuck_cnt=0;

static void Get_TASK(void *pdata)
{
	while(1)
	{
		if(ifGet)
		{
			Valve_Ctrl(VCtr_Board);
			
			DJmotor[DJ2006LR].begin=1;
			DJmotor[DJ2006LR].mode=DJZERO;
			
			Tmotor[0].mode=TVel;
			Tmotor[0].valueSet.kp=0;
			Tmotor[0].valueSet.kd=1;
			Tmotor[0].valueSet.speed=10;
			if((Tmotor[0].valueReal.Preanlge-Tmotor[0].valueReal.angle)<0.1)
				Tmotor_stuck_cnt++;
			else
				Tmotor_stuck_cnt=0;
			if(Tmotor_stuck_cnt>15)
			{
				Tmotor[0].mode=TPos;
				Tmotor[0].valueSet.angle=Tmotor[0].valueReal.angle;
				Tmotor_stuck_cnt=0;
				ifGet=0;
				DJmotor[DJ2006LR].begin=1;
				DJmotor[DJ2006LR].mode=DJZERO;
			}

			OSTimeDly(1000);
			Zdrive[DsRoller].begin=1;
			Zdrive[DsRoller].mode=Zdrive_Speed;
			Zdrive[DsRoller].ValueSet.speed=-300;
			Zdrive[DsBeltFront].begin=1;
			Zdrive[DsBeltFront].mode=Zdrive_Speed;
			Zdrive[DsBeltFront].ValueSet.speed=-300;
			Zdrive[DsBeltBack].begin=1;
			Zdrive[DsBeltBack].mode=Zdrive_Speed;
			Zdrive[DsBeltBack].ValueSet.speed=-200;
		}
		
		OSTimeDly(200);

	}
}	



static void Identify_TASK(void *pdata)
{
	while(1)
	{
		if(ifIdentify)
		{
			Zdrive[DsRoller].ValueSet.speed=0;
			Zdrive[DsBeltFront].ValueSet.speed=0;
			Zdrive[DsBeltBack].ValueSet.speed=0;
			switch (Balltype)
			{
				case BadBall:
				{
					Valve_Ctrl(VCtr_Stick|VCtr_Board);
					Zdrive[DsBeltBack].begin=1;
					Zdrive[DsBeltBack].mode=Zdrive_Speed;
					Zdrive[DsBeltBack].ValueSet.speed=-200;
					OSTimeDly(2000);
					Valve_Ctrl(0);
					Zdrive[DsBeltBack].ValueSet.speed=0;
					Balltype=Nothing;
					ifIdentify=false;
				}break;

				case GoodBall:
				{
					Valve_Ctrl(VCtr_Claw);

					if(ifIdentify&0x0f)
					{
						Tmotor[0].mode=TPosVel;
						Tmotor[0].valueSet.angle=0;
						Tmotor[0].valueSet.kp=1;
						Tmotor[0].valueSet.kd=0.2;
						Tmotor[0].valueSet.speed=-30;
						if((Tmotor[0].valueReal.Preanlge-Tmotor[0].valueReal.angle)<0.1)
							Tmotor_stuck_cnt++;
						else 
							Tmotor_stuck_cnt=0;
						if(Tmotor_stuck_cnt>15)
						{
							Tmotor_SetZero(1);
							Tmotor[0].mode=TPos;
							Tmotor[0].valueSet.angle=0;
							Tmotor_stuck_cnt=0;
							ifIdentify=ifIdentify&0xf0;
						}
					}

					DJmotor[DJ3508UD].begin=1;
					DJmotor[DJ3508UD].mode=DJZERO;
					DJmotor[DJ3508UD].Limit.ZeroSP=-2000;
					if(	DJmotor[DJ3508UD].valueReal.angle	<	(-300)	)
					{
						DJmotor[DJ2006LR].begin=1;
						DJmotor[DJ2006LR].mode=DJPOSITION;
						DJmotor[DJ2006LR].valueSet.angle=1100;
						ifIdentify=ifIdentify&0x0f;
						
					}
				}break;
			
				case Nothing:
					break;
				default:
					break;
			}
		}
		if(!ifIdentify)
		Balltype=Nothing;
		OSTimeDly(200);
	}
}

static void Put_TASK(void *pdata)
{
	while(1)
	{
		if(ifPut)
		{
			Valve_Ctrl(0);
			OSTimeDly(1000);
			DJmotor[DJ3508UD].begin=1;
			DJmotor[DJ3508UD].mode=DJZERO;
			DJmotor[DJ3508UD].Limit.ZeroSP=2000;
			DJmotor[DJ2006LR].begin=1;
			DJmotor[DJ2006LR].mode=DJPOSITION;
			DJmotor[DJ2006LR].valueSet.angle=0;
	
			ifPut=false;
		}
		OSTimeDly(200);
	}
}

static void Ctr_Task(void *pdata)
{
	while(1)
	{
//			Tmotor[0].enable=ifEnable;
//			Zdrive[DsBeltBack].enable=ifEnable;
//			Zdrive[DsBeltFront].enable=ifEnable;
//			Zdrive[DsRoller].enable=ifEnable;
//			DJmotor[DJ3508UD].enable=ifEnable;
//			DJmotor[DJ2006LR].enable=ifEnable;
//			DJmotor[DJ2006CAMERA].enable=ifEnable;

//			if(ifAllFindZero) ;
			if(ifAllSetZero)		
			{
				DJ_SetZero(DJ2006CAMERA);
				DJ_SetZero(DJ2006LR);
				DJ_SetZero(DJ3508UD);
				Tmotor_SetZero(1);
				ifAllSetZero=0;
			}	
		OSTimeDly(200);
	}
}

static void Task_BEEP(void *pdata)
{
	pdata = pdata;
	for (;;)
	{
			OSTimeDly(2000);
	}
}

u8 haha;
static void Task_LED(void *pdata)
{
	while(1)
	{
		Valve_Ctrl(0XFF);
		OSTimeDly(2000);
	}
}

/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑       永不宕机     永无BUG
 */
