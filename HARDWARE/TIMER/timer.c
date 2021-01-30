/************************************************************
 *File		:	timer.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Timer5: 1ms, for Control_Task()
								Timer2: 1ms, for IMU Calculation
								Timer12: 50HZ, for Friction Wheel control
 ************************************************************/

#include "main.h"

#define Init_time 3000 

u16 TIM6_time_count = 0;  //TIM6计时
u16 TIM6_time_count_1 = 0;  //
u8 flag_Ready = 0; //初始化完成标志


/*-------------  定时器6初始化  -------------*/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///使能TIM6时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//初始化TIM3
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn ; //定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6,ENABLE); //使能定时器6
	
}

/*-------------  定时器6中断服务  -------------*/
void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //溢出中断
	{			
		if(!flag_Ready)
		{
			if (TIM6_time_count < Init_time)   //等3秒，使初始化完成，并使云台以较慢速度归中，此时运行PREPARE_STATE模式，不接收遥控器数据，见函数Remote_State_FSM()
			{
				TIM6_time_count++;
			}
			else 
			{			 
			 TIM6_time_count = 0;
			 flag_Ready = 1;
			}
		}
		continue_value();
		TIM6_time_count_1++;
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //清除中断标志位
}



