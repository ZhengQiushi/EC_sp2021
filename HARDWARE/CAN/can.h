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
void continue_value(void);
void CAN1_Cmd(int16_t l_data1,int16_t l_data2,int16_t R_data1,int16_t R_data2);


volatile extern int16_t current_position_201; 
volatile extern int16_t current_speed_201;
volatile extern float continuous_current_position_201;
volatile extern int16_t current_position_202; 
volatile extern int16_t current_speed_202;
volatile extern float continuous_current_position_202;
volatile extern int16_t current_position_203; 
volatile extern int16_t current_speed_203;
volatile extern float continuous_current_position_203;
volatile extern int16_t current_position_204; 
volatile extern int16_t current_speed_204;
volatile extern float continuous_current_position_204;



#endif

















