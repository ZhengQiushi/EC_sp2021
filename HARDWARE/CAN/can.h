#ifndef __CAN_H
#define __CAN_H	 
#include "stm32f4xx.h"
//#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
void CAN2_Cmd_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4);
void CAN1_Cmd_standard(void);
void CAN1_Cmd_All(int16_t current_205,int16_t current_206);
void CAN1_Cmd_Confirm(int16_t current_205,int16_t current_206,int16_t current_207);
void CAN1_Cmd_Cap(u8 mode,int16_t set_val);
void Cap_Update_Cmd(u8 module_state, u8 cap_state,u8 val_change_state,u16 set_value);
void continue_value(void);
extern int count_207_temp;
void CAN2_Cmd_Float_Clear(void);
void CAN2_Send_Clear(int16_t clear_angle);
void CAN1_fri_Cmd(int16_t l_data,int16_t r_data);
void CAN1_Feed_Cmd(int16_t l_data);
void Speed_Cmd(void);


typedef struct
{
	uint8_t charge_enabled_state;
	uint8_t residue_power;
	uint32_t charge_power;
	int32_t chassis_power;
	int32_t cap_vol;
}supercap_module_receive;

#define SCM_rx_mes supercap_module_receive  //改个名

volatile extern int16_t current_position_206;    	//pitch gimbal
volatile extern int16_t current_position_205;			//yaw gimbal
volatile extern int16_t current_position_207;
volatile extern int16_t current_speed_207;
volatile extern int16_t current_cm_201;
volatile extern int16_t current_cm_202;
volatile extern int16_t current_cm_203;
volatile extern int16_t current_cm_204;
volatile extern int16_t current_cm_205;
volatile extern int16_t current_cm_206;
volatile extern int16_t continuous_current_position_205;			//yaw gimbal
volatile extern int16_t continuous_current_position_206;    	//pitch gimbal
volatile extern float continuous_current_position_207;
extern SCM_rx_mes SCM_rx_message;
volatile extern int16_t current_fric_ti_201;   //201 左摩擦轮
volatile extern int16_t current_fric_ti_202;   //202 右摩擦轮
volatile extern int16_t current_fric_s_201;
volatile extern int16_t current_fric_s_202;
volatile extern int16_t t_i_1;
volatile extern int16_t t_i_2;
volatile extern int16_t t_i_3;
volatile extern int16_t t_i_4;
volatile extern int16_t continuous_current_position_filtered_205;			//yaw gimbal
volatile extern int16_t continuous_current_position_filtered_206;    	//pitch gimbal
extern int16_t pitch_history[20];
extern int16_t yaw_speed;
extern int16_t aid_dynamic_mach_angle;
#endif

















