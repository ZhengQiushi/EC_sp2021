#ifndef __MAIN_H_
#define __MAIN_H_



#include "sp_type.h"
#include "RefereeInfo.h"
#include "sp_math.h"
#include "sp_pid.h"
#include "sp_filter.h"
//#include "frame.h"

#include "bsp.h"
#include "delay.h"
#include "buzzer.h"
#include "led.h"
#include "spi.h"
#include "adi_gyro.h"
#include "can.h"
#include "euler.h"
#include "rc.h"
#include "timer.h"
#include "usb.h"
#include "usart2.h"
#include "usart3.h"
#include "usart6.h"
#include "uart8.h"
//#include "jy901_usart.h"
#include "laser.h"
#include "mpu6500.h"

//#include "pid.h"

#include "MonitorControl.h"
#include "RemoteControl.h"
#include "ControlTask.h"
#include "cap_control.h"
#include "CMControl.h"
#include "FrictionControl.h"
#include "FeedControl.h"
#include "GimbalControl.h"
#include "Auto_aim.h"
#include "SendCom.h"


#define Referee_System //接入裁判系统时开启


#define INFANTRY_Jiangsu 

#define BLUE_TEAM
//#defien RED_TEAM

#define tiaocan


#define _chassis_move
#define Gimbal_Move

#define AutoAim_USB
//#define USING_USB

//#define SUPER_CAPACITOR
// Power_Limitation
//#define Cap_Offline_Debug

#define hengfang

#define gongkong


#define GYRO_X_OFFSET 0//0.0390f  
#define GYRO_Y_OFFSET 0//0.05412f
#define GYRO_Z_OFFSET 0//-0.037880f




/*****************************************************/
#ifdef INFANTRY_Jiangsu

#define MIDDLE_YAW 	7132 //2000 
#define MIDDLE_PITCH  4800 //4930

#define MOUSE_YAW_SPEED 0.03 //0.0024
#define MOUSE_PIT_SPEED 1.2    //0.1

#define XLIMIT_OFFSET 400
#define YLIMIT_OFFSET 500
#define gears_speedXYZ 	18
#define gears_speedRAD 	15
#define followVal_limit 40

#define COVER_OPEN 1700
#define COVER_CLOSE 1000

#define SHOOT_UP_HIGH_INIT 103
#define SHOOT_UP_LOW_INIT 52


#define YawMax 1200  //+-1200
#define YawMin -1200
#define PitMax 800
#define PitMin -650

#define ZERO_FLOAT_INIT -20

#ifdef RED_TEAM
	#define MY_ROBOT_ID robotid_red_infantry_3
	#define MY_CLIENT_ID clientid_red_infantry_3//红方3号步兵
#endif

#ifdef BLUE_TEAM
	#define MY_ROBOT_ID robotid_blue_infantry_3
	#define MY_CLIENT_ID clientid_blue_infantry_3//蓝方3号步兵
#endif

#endif
/*****************************************************/
#endif




