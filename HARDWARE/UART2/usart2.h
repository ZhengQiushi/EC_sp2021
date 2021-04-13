#ifndef __USART2_H__
#define __USART2_H__

#include "stm32f4xx.h"

#define AUTO_FRAME_HEAD 0xf1
#define AUTO_FRAME_LENGTH 19//15

//typedef __packed struct _frame			//�Ӿ����������ݿ��	
//{
//	u16 head;		//֡ͷ��Ϊ0xf1f1
//	u16 timestamp;//�����
//	float yaw;	  //yaw���ֵ
//	float pitch;  //pitch���ֵ
//	u8 extra[2]; //additional imformation	
//	u8 crc8check;	//crc8У��λ	
//	u8 update_flag;
//}frame;

//typedef struct 			//�Ӿ����������ݿ��	
//{
//	float yaw;	  //yaw���ֵ
//	float pitch;  //pitch���ֵ
//	float yaw_current;
//	float pitch_current;
//	u8 update_flag;
//	u8 shoot_flag;
//	
//}frame_update;.

#pragma  pack(1) 
typedef struct _frame				
{
	u8 head;		//0xf1 0xf1
	u16 timestamp;
	float yaw;	
	float pitch;
	float speed;
	u8 extra[2]; //additional imformation	
	u8 crc8check;
	u8 end;
}frame;			
#pragma  pack() 


void sendtoComputer(int timestamp_doing, int auto_aim, int big_buff,int entering_auto_aim);


void USART2_DMA_Init(void);
u8 crc8Check(u8 *buff,u16 len);
extern u8 update_flag;
extern frame fram;


#endif
