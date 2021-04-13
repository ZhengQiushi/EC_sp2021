/************************************************************
 *File		:	timer.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Timer5: 1ms, for Control_Task()
								Timer2: 1ms, for IMU Calculation
								Timer12: 50HZ, for Friction Wheel control
 ************************************************************/

#include "main.h"

#define Init_time 3000 

u16 TIM6_time_count = 0;  //TIM5��ʱ
u8 flag_Ready = 0; //��ʼ����ɱ�־
float adi_checksum=0;
float adi_checksum_pre=0;
u8 adi_die_flag=0;
u8 adi_die_count=0;

/*-------------  ��ʱ��5��ʼ��  -------------*/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn ; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6,ENABLE); //ʹ�ܶ�ʱ��5
	
}

/*-------------  ��ʱ��5�жϷ���  -------------*/
void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //����ж�
	{			
		if(!flag_Ready)
		{
			if (TIM6_time_count < Init_time)   //��3�룬ʹ��ʼ����ɣ���ʹ��̨�Խ����ٶȹ��У���ʱ����PREPARE_STATEģʽ��������ң�������ݣ�������Remote_State_FSM()
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
			Control_Task();             //������ѭ��
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //����жϱ�־λ
}

/*-------------  ��ʱ��2��ʼ��  -------------*/
void TIM2_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM2ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = 2000; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=90-1;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��2�����ж�
	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}





void TIM2_IRQHandler(void)      //2ms����һ���ж�
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{

		
			update_euler_mpu();
			
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	
}

///************************************************************
//*��ʱ��ʼ�������ڿ��ƶ��
//*************************************************************/
void TIM9_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitSruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  ///TIM3ʱ��ʹ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTHʱ��	
	
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
	
	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse =0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM9, ENABLE);
	TIM_SetCompare1(TIM9, COVER_CLOSE);
}


