/**
  ******************************************************************************
  * @file       sp_shoot.c
  * @author     LSS
  * @version    v0.0-alpha
  * @date       2019.Mar.12
  * @brief      shooting control
  * @usage      
  ******************************************************************************
  * @license
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sp_shoot.h"
#include "Auto_aim.h"
#include "referee.h"

/** @addtogroup SP
  * @{
  */

/** @defgroup   Shoot
  * @{
  */

 
#define Friction_SPEED_p               500.0f
#define Friction_SPEED_i               0.0f
#define Friction_SPEED_d               1.0f
#define Friction_INTE_limitI           60.f
#define DELTA_TIME                     0.01f
#define USING_FRICTION_FILTER                    /*<! Using input filter */

float Feed_SPEED = 20.f;
float max_shoot_speed = 20.f;
static int Shoot_Cooling_Time = 200;
static int Cooling_tickets = 400;

MOTOR_CrtlType_CAN* Friction_CH1;
MOTOR_CrtlType_CAN* Friction_CH2;
MOTOR_CrtlType_CAN* Feed_motor;

RobotMode robotMode=		STANDBY_MODE;
RobotMode robotMode_ex=	STANDBY_MODE;

extern RC_DataType recv;

/**
  * @brief  
  * @note   
  * @param  
  * @retval 
  */ 
// 1.615*set+53.87  = 10^6/counter (div8)
float COMPENSATE  = -0.f;
float MAFilter_Threshold = 50.f;

float delta_lim = 2;
float dspd_gain = 1.6f, dspd_gain2 = 0.0f;

void Friction_Init(void) {
	Friction_CH1 = spMOTOR.user.enable(CAN1, Motor201, RM_3508_P19,false);;
	Friction_CH2 = spMOTOR.user.enable(CAN1, Motor202, RM_3508_P19,false);;
	
	PID_ControllerInit(Friction_CH1->control.speed_pid, Friction_INTE_limitI, (uint16_t)-1, 1000);
	PID_ControllerInit(Friction_CH2->control.speed_pid, Friction_INTE_limitI, (uint16_t)-1, 1000);
	
	PID_SetGains(Friction_CH1 ->control.speed_pid, Friction_SPEED_p, Friction_SPEED_i, Friction_SPEED_d);
  PID_SetGains(Friction_CH2->control.speed_pid, Friction_SPEED_p, Friction_SPEED_i, Friction_SPEED_d);
  
	Friction_CH1 ->control.speed_pid->intergration_separation = Friction_CH2->control.speed_pid->intergration_separation = 20.f;
}



uint8_t DELTA_SPEED = 3;
float SPEED_THRESHOLD = 4;
uint32_t check = 0;
void Friction_Looper(void) {
    static uint16_t speed_shoot = 0;
		static uint16_t timeticket = 0;
		if(frictionState == Friction_OFF) {
				speed_shoot = 0;
				timeticket = 0;
		} 
		else if(frictionState == Friction_ON){
				frictionState = Friction_ON;
				timeticket++;
				if(timeticket > 100){
				if(timeticket%15 == 0)
						speed_shoot += (speed_shoot<max_shoot_speed)?10:0;
				}
				if(timeticket>10000)
					timeticket = 100;
		}
 
//		Friction_CH1->control.speed_pid->target = Friction_CH2->control.speed_pid->target = speed_shoot;
		spMOTOR.user.set_speed(CAN1, Motor201,-speed_shoot);
		spMOTOR.user.set_speed(CAN1, Motor202, speed_shoot);
}

void Shooting_Control_Init (void){
        #ifdef USING_FRICTION
					Friction_Init();
					extern uint16_t frictionState;
        #endif
        
        #ifdef USING_FEED_MOTOR
					MOTOR_CrtlType_CAN* motor203 = spMOTOR.user.enable(CAN1, Motor203, RM_2006_P36, false);
					motor203->control.speed_pid->Kp = 500.0f;
					motor203->control.speed_pid->Ki = 0.0f;
					motor203->control.speed_pid->Kd = 1.f;
					motor203->control.speed_pid->intergrations_sum_error_limit = 5*PI;
					motor203->control.speed_pid->intergration_separation = PI;
					motor203->control.output_limit = 9000;
					Feed_motor = motor203;
        #endif
}

void Feed_Motor_ON(void){
	spMOTOR.user.set_speed(CAN1, Motor203, -Feed_SPEED);
}

void Feed_Motor_OFF(void){
	spMOTOR.user.set_speed(CAN1, Motor203, 0);
}

void Feed_Motor_BACK(void){
	spMOTOR.user.set_speed(CAN1, Motor203, Feed_SPEED);
}

void Feed_Motor_Looper(void){
	static int16_t stop_flag = 0;
	static int16_t times = 0;
	static float motor_position = 0;
	static float motor_position_ex = 0.1f;
	motor_position = Feed_motor->state.current;
	if(shootState == Shoot_OFF){
		Feed_Motor_OFF();
	}
	if(shootState == Shoot_ON){
		if(stop_flag == 0){
			Feed_Motor_ON();
		}
		else{
			times++;
			if(times<50){
				Feed_Motor_BACK();
			}
			else if(times<100){
				Feed_Motor_ON();
			}
			else{
				times = 0;
				stop_flag = 0;
			}
		}
	 static int16_t times2 = 0;
		if(fabs(motor_position - motor_position_ex) < 10.0f/8192.f*2.f*PI && stop_flag == 0){
			times2++;
			if(times2 > 3){
				stop_flag = 1;
				times2 = 0;
			}
		}
		else{
			times2 = 0;
		}
		
	}
	motor_position_ex = motor_position;
}

void Shooting_Control_Looper (void){
		static RC_DataType recv_ex;
		#ifdef USING_FRICTION
			if(recv.rc.s2 == RC_SW_MID) {
					frictionState = Friction_OFF;
			} 
			else if(recv.rc.s2==RC_SW_DOWN){
					if(recv.rc.s1==RC_SW_MID  && recv_ex.rc.s1==RC_SW_UP) {
							frictionState = Friction_ON;
					}
			}
			else if(recv.rc.s2==RC_SW_UP){
				frictionState = Friction_ON;
			}
			Friction_Looper();
		#endif
			
		#ifdef USING_FEED_MOTOR
			if(recv.rc.s2 == RC_SW_MID) {
				shootState = Shoot_OFF;
			} 
			else if(recv.rc.s2==RC_SW_DOWN){
				if(recv.rc.s1==RC_SW_DOWN)
					shootState = Shoot_ON;
				else
					shootState = Shoot_OFF;
				
#if defined(BOARD_MODE) && BOARD_MODE == 1
					if(ext_power_heat_data.shooter_heat0 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat0 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}
#elif defined(BOARD_MODE) && BOARD_MODE == 2
					if(ext_power_heat_data.shooter_heat1 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat1 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}					
#endif
				if(Cooling_tickets < Shoot_Cooling_Time){
					shootState = Shoot_OFF;
					Cooling_tickets++;
				}
				else if(Cooling_tickets > 10000)
					Cooling_tickets = Shoot_Cooling_Time;
				
			}
			else if(recv.rc.s2==RC_SW_UP){
				if(recv.rc.s1==RC_SW_DOWN ||recv.rc.s1==RC_SW_MID){
					shootState = Shoot_OFF;
					static float times_tick = 0;
					if(recv.rc.ch0 > 600 && times_tick < 20){
						shootState = Shoot_ON;
						times_tick++;
					}
					if(recv.rc.ch0 < 600){
						times_tick = 0;
					}
					
					if(recv.rc.ch0 < -600){
						shootState = Shoot_ON;
					}
					
#if defined(BOARD_MODE) && BOARD_MODE == 1
					if(ext_power_heat_data.shooter_heat0 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat0 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}
#elif defined(BOARD_MODE) && BOARD_MODE == 2
					if(ext_power_heat_data.shooter_heat1 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat1 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}					
#endif
					if(Cooling_tickets < Shoot_Cooling_Time){
						shootState = Shoot_OFF;
						Cooling_tickets++;
					}
					else if(Cooling_tickets > 10000)
						Cooling_tickets = Shoot_Cooling_Time;
				}
				else if(recv.rc.s1==RC_SW_UP){
					static float auto_aim_keep;
					if(auto_aim_flag == 1){
						shootState = Shoot_ON;
						auto_aim_keep = 30;
					}
					else
						shootState = Shoot_OFF;
					if(auto_aim_keep > 0){
						shootState = Shoot_ON;
						auto_aim_keep --;
					}
					static float times_tick = 0;
					if(recv.rc.ch0 > 600 && times_tick < 20){
						shootState = Shoot_ON;
						times_tick++;
					}
					if(recv.rc.ch0 < 600){
						times_tick = 0;
					}
					
					if(recv.rc.ch0 < -600){
						shootState = Shoot_ON;
					}
#if defined(BOARD_MODE) && BOARD_MODE == 1
					if(ext_power_heat_data.shooter_heat0 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat0 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}
#elif defined(BOARD_MODE) && BOARD_MODE == 2
					if(ext_power_heat_data.shooter_heat1 > 440){
						shootState = Shoot_OFF;
						Cooling_tickets = 0;
					}
					if(ext_power_heat_data.shooter_heat1 == 0){
						Cooling_tickets = Shoot_Cooling_Time;
					}					
#endif
					if(Cooling_tickets < Shoot_Cooling_Time){
						shootState = Shoot_OFF;
						Cooling_tickets++;
					}
					else if(Cooling_tickets > 10000)
						Cooling_tickets = Shoot_Cooling_Time;
				}
			}
			if(frictionState == Friction_OFF)
				shootState = Shoot_OFF;
			Feed_Motor_Looper();
		#endif
		recv_ex = recv;
}

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT Tongji Super Power *****END OF FILE****/
