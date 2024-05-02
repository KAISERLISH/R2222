#ifndef _MAIN_H
#define _MAIN_H

#include "stm32f4xx.h"
#include "includes.h"

#include "can1.h"
#include "can2.h"
#include "tim2.h"
#include "tim3.h"
#include "led.h"
#include "beep.h"
#include "param.h"
#include "DJmotor.h"
#include "cylinder.h"
#include "mech.h"

#define START_TASK_PRIO 5
#define Get_Task_PRIO 10
#define Identify_Task_PRIO 11
#define Put_Task_PRIO 12
#define Ctr_Task_PRIO 13
#define	BEEP_TASK_PRIO	44	//�����������ȼ�
#define LED_TASK_PRIO 43

#define TASK_STK_SIZE 1024

__align(8) OS_STK START_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK Get_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK Identify_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK Put_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK Ctr_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK BEEP_TASK_STK[TASK_STK_SIZE];	//�����ջ
__align(8) OS_STK LED_TASK_STK[TASK_STK_SIZE];	//�����ջ


static void Task_Start(void *pdata);	//������
static void Get_TASK(void *pdata);	//������
static void Identify_TASK(void *pdata);	//������
static void Put_TASK(void *pdata);	//������
static void Ctr_Task(void *pdata);	//������
static void Task_BEEP(void *pdata);	//������
static void Task_LED(void *pdata);	//������


#endif
