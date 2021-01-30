#include "main.h"



PID_Type M201_Position_PID,M202_Position_PID,M203_Position_PID,M204_Position_PID;

float M201_PID_PP=90;//40.0;
float M201_PID_PI=4.2;//3.2;
float M201_PID_PD=0;//1;
float PIDOut_Position_201;

float M202_PID_PP=70;//40.0;
float M202_PID_PI=4.2;//3.2;
float M202_PID_PD=0;//1;
float PIDOut_Position_202;

float M203_PID_PP=90;//40.0;
float M203_PID_PI=4.2;//3.2;
float M203_PID_PD=0;//1;
float PIDOut_Position_203;

float M204_PID_PP=70;//40.0;
float M204_PID_PI=4.2;//3.2;
float M204_PID_PD=0;//1;
float PIDOut_Position_204;

void Pid_M201_Init(void)
{
	
	PID_ControllerInit(&M201_Position_PID,50,50,9800,0.01);
	PID_SetGains(&M201_Position_PID,M201_PID_PP,M201_PID_PI,M201_PID_PD);
	
	PID_ControllerInit(&M202_Position_PID,50,50,9800,0.01);
	PID_SetGains(&M202_Position_PID,M202_PID_PP,M202_PID_PI,M202_PID_PD);
	
	PID_ControllerInit(&M203_Position_PID,50,50,9800,0.01);
	PID_SetGains(&M203_Position_PID,M203_PID_PP,M203_PID_PI,M203_PID_PD);
	
	PID_ControllerInit(&M204_Position_PID,50,50,9800,0.01);
	PID_SetGains(&M204_Position_PID,M204_PID_PP,M204_PID_PI,M204_PID_PD);
}


void M_pid(void)
{	
	float NowPosition_201=continuous_current_position_201;
	float NowSpeed_201= current_speed_201;
	
	float NowPosition_202=continuous_current_position_202;
	float NowSpeed_202= -current_speed_202;
	
	float NowPosition_203=continuous_current_position_203;
	float NowSpeed_203= current_speed_203;
	
	float NowPosition_204=continuous_current_position_204;
	float NowSpeed_204= -current_speed_204;
#ifdef Fengche_smallbuff
	
	PIDOut_Position_201=PID_ControllerDriver(&M201_Position_PID,7,NowSpeed_201);
	PIDOut_Position_202=PID_ControllerDriver(&M202_Position_PID,-7,NowSpeed_202);
	PIDOut_Position_203=PID_ControllerDriver(&M203_Position_PID,5,NowSpeed_203);
	PIDOut_Position_204=PID_ControllerDriver(&M204_Position_PID,-5,NowSpeed_204);
	CAN1_Cmd(PIDOut_Position_201,PIDOut_Position_202,PIDOut_Position_203,PIDOut_Position_204);	
	
#endif	
	
#ifdef Fengche_bigbuff	
	PIDOut_Position_201=PID_ControllerDriver(&M201_Position_PID,9.55*(0.785f*sin(1.884f*TIM6_time_count_1/1000)+1.305f),NowSpeed_201);
	PIDOut_Position_202=PID_ControllerDriver(&M202_Position_PID,9.55*(0.785f*sin(1.884f*TIM6_time_count_1/1000)+1.305f),NowSpeed_202);
	CAN1_Cmd(PIDOut_Position_201,PIDOut_Position_202);	

#endif

	
}



