/*\
  ******************************************************************************
  * @author         : Crazt
  * @brief          : Control Task Loop , Use Tim1
  * 				  For Tongji SuperPower 2020
  ******************************************************************************

  ******************************************************************************
 */
#include"ControlTask.h"
#include"WS2812.h"
#include"SCM.h"
#include"ina226.h"
#include"upper.h"
#include"mpu9255.h"

#include "stdio.h"
#include "spi.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "can.h"

#include "math.h"

uint16_t TimeTick=0;
uint16_t BreathLed=0,ButtomCD=0;
uint8_t  DirFlag=0,ButtomState=0;

void ControlTask(void)
{

	IMU_TASK();
	if(TimeTick %10==0)
		ButtonSense();
	if(TimeTick %100==3)
		SCM_ControlLoop();

	if(TimeTick %50==25){
		Ina226_Read();
		SCM_MonitorUpdate();
		SCM_CAN_Transmit();
	}
	if(TimeTick %50==15){
//		MPU9255_Print();
//		printf("SetPower=%d,RealPower=%d,CapVol=%d\r\n",(int)SCM.SetPow,(int)SCM.CapPow,(int)SCM.CapVol);
//		printf("SetCur=%d,CapCur=%d,CapVol=%d\r\n",(int)SCM.SetCur,(int)SCM.CapCur,(int)SCM.CapVol/10);
//		printf("ChasisPower=%f,CapPower=%f\r\n",((float)SCM.BatPow)/1000.0,(float)SCM.CapPow/1000.0);
		if(UartTransmitEnableTime>0)
			USER_UART1_DMA_Transmit2Upper();


	}
	if(TimeTick%100==35)
	{
		WS2812SetColor(0,20,20,20);
		if(SCM.CAN_ConnectWDT==0)
			WS2812SetColor(1,40,0,0);
		else
			WS2812SetColor(1,0,60,0);
		if(SCM.CapVol<SCM_Settings.Cap_MinVol_L)
			WS2812SetColor(2,(uint8_t)40.0*((float)SCM.CapVol/(float)SCM_Settings.Cap_MinVol_L),0,0);
		else
			WS2812SetColor(2,0,0,(uint8_t)100.0*(float)(SCM.CapVol-SCM_Settings.Cap_MinVol_L)/(float)(SCM_Settings.Cap_MaxVol_H-SCM_Settings.Cap_MinVol_L));
		RGB_LedsArrayTransmit();
	}
	if(TimeTick %500==27)
		SCM_Display();

	if(UartTransmitEnableTime>0)
		UartTransmitEnableTime--;
	//TimeTick Update
	TimeTick++;
	if(TimeTick>=10000)	TimeTick=0;
	if(ButtomCD>0) ButtomCD--;
	//BreathLed
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, BreathLed); //Set dutyCycle
	if(DirFlag==1)
		BreathLed--;
	else
		BreathLed++;
	if(BreathLed <=500)
		{DirFlag=0;BreathLed=500;}
	if(BreathLed >=2000)
		{DirFlag=1;BreathLed=2000;}

}
void ButtonSense(void){
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)==GPIO_PIN_RESET && ButtomCD ==0){
		ButtomState=1;
		ButtomCD=1000;
	}
	if(ButtomState==1){
		SCM_Next.Buck=0;
		SCM_Next.SetPow=0;
		ButtomState=0;
	}

}
