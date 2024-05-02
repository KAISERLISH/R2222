#ifndef _MECH_H
#define _MECH_H

#include "stm32f4xx.h"
#include "queue.h"
#include "param.h"
#include "stdarg.h"
#include "Tmotor.h"
#include "cylinder.h"
#include "tim3.h"
#include "Tmotor.h"

#define DsRoller 1
#define DsBeltFront 2
#define DsBeltBack 3

#define DJ3508UD 0
#define DJ2006LR 1
#define DJ2006CAMERA 2

#define AK60G6ID 0

#define RollerPosDown 00
#define RollerSpeed 00

#define BeltFrontSpeed 00
#define BeltBackSpeed 00

#define VCtr_Board 0x00
#define VCtr_Stick 0x10
#define VCtr_Claw 0x40

#define ClawPosXA 0
#define ClawPosXB 1100
#define ClawPosUpMid -300

//extern TmotorValueTypedef;
#endif
