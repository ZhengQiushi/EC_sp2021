#ifndef __Gimbal_Control_H
#define __Gimbal_Control_H
#include "sp_pid.h"

void M_pid(void);
void Pid_M201_Init(void);

typedef struct
{
	PID_Type Position;
	PID_Type Speed;	
}GimbalPID;

#endif



