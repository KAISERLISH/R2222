#include "pid.h"

void Inc_PID_Init(Inc_PIDTypedef* PID,float KP,float KI,float KD,int Set)
{
	PID->CurVal=0;
	PID->SetVal=Set;
	PID->midVal[0]=0;
	PID->midVal[1]=0;
	PID->midVal[2]=0;
	PID->Kp=KP;
	PID->Ki=KI;
	PID->Kd=KD;
}

void Inc_PID_Operation(Inc_PIDTypedef* PID)
{
	PID->midVal[0]=PID->SetVal - PID->CurVal;
	PID->delta = PID->Kp * (PID->midVal[0]-PID->midVal[1]) + PID->Ki * PID->midVal[0] + PID->Kd * (PID->midVal[0] + PID->midVal[2] - 2*PID->midVal[1]);
	PID->midVal[2]=PID->midVal[1];
	PID->midVal[1]=PID->midVal[0];
}

