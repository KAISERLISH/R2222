#ifndef _PID_H
#define _PID_H


#include "stm32f4xx.h"
typedef struct
{
	vs32 SetVal;
	float Kp;
	float Ki;
	float Kd;
	vs32 delta;
	vs32 CurVal;
	vs32 midVal[3];
}Inc_PIDTypedef;

typedef struct{
	float kp;
	float ki;
	float kd;
	float kd_filter;
	float d_filter;
	float p_term;
	float i_term;
	float d_term;
	s32 error;
	s32 prv_error;
	float output;
	s32 CurVal;
	s32 SetVal;
}VESC_PIDTypedef;



void Inc_PID_Init(Inc_PIDTypedef* PID,float KP,float KI,float KD,int Set);
void Inc_PID_Operation(Inc_PIDTypedef* PID);

#endif
