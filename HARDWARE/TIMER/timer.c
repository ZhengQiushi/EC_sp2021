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

u16 TIM6_time_count = 0;  //TIM5计时
u8 flag_Ready = 0; //初始化完成标志
float adi_checksum=0;
float adi_checksum_pre=0;
u8 adi_die_flag=0;
u8 adi_die_count=0;

/*-------------  定时器5初始化  -------------*/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//初始化TIM3
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn ; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6,ENABLE); //使能定时器5
	
}

/*-------------  定时器5中断服务  -------------*/
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
			Control_Task();             //主任务循环
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //清除中断标志位
}

/*-------------  定时器2初始化  -------------*/
void TIM2_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM2时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 2000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM_Cmd(TIM2,ENABLE); //使能定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}





void TIM2_IRQHandler(void)      //2ms产生一次中断
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{

		
			update_euler_mpu();
			
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	
}

///************************************************************
//*定时初始化，用于控制舵机
//*************************************************************/
void TIM9_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitSruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  ///TIM3时钟使能	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTH时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM9);
	
	TIM_TimeBaseInitSruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitSruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitSruct.TIM_Period=10000-1;
	TIM_TimeBaseInitSruct.TIM_Prescaler=180-1;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitSruct);
	
	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse =0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能 
	TIM_Cmd(TIM9, ENABLE);
	TIM_SetCompare1(TIM9, COVER_CLOSE);
}


