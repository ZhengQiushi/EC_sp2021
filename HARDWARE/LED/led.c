/************************************************************
 *File		:	led.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@Jason ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	RM2018_INFANTRY project. 
		            Led GPIO Init.
								PF14 -> LED_red
								PE7  -> LED_green
 *Input   : None
 *Output  : None
 ************************************************************/

#include "main.h"


void Power_Configuration(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH| RCC_AHB1Periph_GPIOG , ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOH, &GPIO_InitStructure);//初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	GPIO_SetBits(GPIOG,GPIO_Pin_13);
	GPIO_SetBits(GPIOH,GPIO_Pin_5);
	GPIO_SetBits(GPIOH,GPIO_Pin_4);
	GPIO_SetBits(GPIOH,GPIO_Pin_3);
	GPIO_SetBits(GPIOH,GPIO_Pin_2);
	

}

void Elc_Push(void)
{
	
	GPIO_SetBits(GPIOH,GPIO_Pin_3);
	delay_ms(5000);
	GPIO_ResetBits(GPIOH,GPIO_Pin_3);
	
}




