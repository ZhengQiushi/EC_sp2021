 /*
  ******************************************************************************
  * @author         : Crazt
  * @brief          : Super capacitor control program
  * 				  For Tongji SuperPower 2020
  ******************************************************************************

  ******************************************************************************
 */
#include "SCM.h"
#include "ina226.h"
#include "ssd1306.h"

#include "dac.h"
#include "string.h"
#include "can.h"

#define DispLen 6

uint8_t SCM_CanReceiveFlag=SCM_CAN_Ready;

SCM_Settings_TypeDef SCM_Settings,SCM_SettingsNext;
SCM_NextStatus_TypeDef SCM_Next;
SCM_StatusTypeDef SCM;


char str[6][9+DispLen];
int32_t DisplayVal[6];
uint32_t TestVal;


void SCM_Init(void){
	SCM.Buck=SCM_Buck_Off;
	SCM.Mode=SCM_Mode_Bat;
	SCM.ForceUseCap=0;
	SCM.SetPow=0;
	SCM.SetCur=0;

	SCM.CapVol=0;
	SCM.CapCur=0;
	SCM.CapPow=0;
	SCM.BatVol=0;
	SCM.BatCur=0;
	SCM.BatPow=0;
	SCM.CAN_ConnectWDT=0;

	SCM_Settings.Cap_MaxVol_H=Cap_MaxVol;
	SCM_Settings.Cap_MaxVol_L=Cap_MaxVolLow;
	SCM_Settings.Cap_MinVol_H=Cap_MinVolHigh;
	SCM_Settings.Cap_MinVol_L=Cap_MinVol;

	SCM_SettingsNext.Cap_MaxVol_H=Cap_MaxVol;
	SCM_SettingsNext.Cap_MaxVol_L=Cap_MaxVolLow;
	SCM_SettingsNext.Cap_MinVol_H=Cap_MinVolHigh;
	SCM_SettingsNext.Cap_MinVol_L=Cap_MinVol;
	//Disable LTC1473 diode mode
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
}

void SCM_ControlLoop(void){
	//SCM data update by CAN Rx interrupt: HAL_CAN_RxFifo0MsgPendingCallback()
	//The 12bits DAC of stm32f103 can't turn off the module totally, so only open buck when SetPow>1000.

	//Status Control
	SCM.ForceUseCap=SCM_Next.ForceUseCap;

	SCM_Settings.Cap_MaxVol_H=SCM_SettingsNext.Cap_MaxVol_H;
	SCM_Settings.Cap_MaxVol_L=SCM_SettingsNext.Cap_MaxVol_L;
	SCM_Settings.Cap_MinVol_H=SCM_SettingsNext.Cap_MinVol_H;
	SCM_Settings.Cap_MinVol_L=SCM_SettingsNext.Cap_MinVol_L;

	//From buck off to buck run
	if(SCM.Buck==SCM_Buck_Off && SCM_Next.Buck==SCM_Buck_Run){
		if(SCM.CapVol<SCM_Settings.Cap_MaxVol_L-1000 && SCM_Next.SetPow>1000){
			SCM.Buck=SCM_Buck_Run;
			SCM.SetPow=SCM_Next.SetPow;
			SCM_SetPower(SCM.SetPow);
		}else if(SCM.CapVol<SCM_Settings.Cap_MaxVol_L&& SCM_Next.SetPow>1000){
			SCM.Buck=SCM_Buck_Run;
			SCM.SetPow=SCM_Next.SetPow;
			SCM_SetPower(SCM.SetPow>25000?25000:SCM.SetPow);
		}
	}else{ //Other status
		SCM.Buck=SCM_Next.Buck;
		SCM.SetPow=SCM_Next.SetPow;
		SCM_SetPower(SCM.SetPow);
	}
	//Cap voltage limitation
	if(SCM.CapVol>SCM_Settings.Cap_MaxVol_H||SCM.SetPow<1000){
		SCM.Buck=SCM_Buck_Off;
		SCM.SetPow=0;
		SCM_SetPower(SCM.SetPow);
	}
	//From Battery mode to Cap Mode
	if(SCM.Mode==SCM_Mode_Bat && SCM_Next.Mode==SCM_Mode_Cap){
		if(SCM.CapVol > SCM_Settings.Cap_MinVol_H)
			SCM.Mode=SCM_Mode_Cap;

	}else
		SCM.Mode=SCM_Next.Mode;
	//Cap Voltage Limitation
	if(SCM.CapVol<SCM_Settings.Cap_MinVol_L)
		SCM.Mode=SCM_Mode_Bat;

	//GPIO Control
	if(SCM.Buck==SCM_Buck_Run)
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);

	if(SCM.Mode==SCM_Mode_Cap||SCM.ForceUseCap){
		SCM.Mode=SCM_Mode_Cap;//强制切换为电容供电
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
	}else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
	}

	SCM_CanReceiveFlag=SCM_CAN_Ready;
	if(SCM.CAN_ConnectWDT>0)
		SCM.CAN_ConnectWDT--;
	//Power Voltage Current monitor status update
	//Location: ControlTask.c
}
//Unit mA
void SCM_SetCurrent(int32_t SetCur){
	//The modified Value change with SetCur actual value, generate a variable SetCurModifiedValue to improve precision.
	uint16_t modified_SetCur=(uint16_t)SetCur/1.19;
	uint16_t val=(modified_SetCur/9600.0)*1.2/3.3*4096;
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
}
//Uint mW
void SCM_SetPower(int32_t SetPow){
	//Set minimum number avoid number/zero, or it will cause hardware fault.
	//???? Observe From test
	SetPow+=7000.0*((float)SCM.CapVol/(float)26000);

	if(SCM.CapVol>18700)
		SetPow+=2300;
	//????
	int32_t CapVol=SCM.CapVol>500?SCM.CapVol:500;
	float factor=0.83f;
	int32_t SetCur=SetPow/(factor*CapVol/1000.0);
	//9000mA is the maximum output current of this module
	SetCur=SetCur>9000?9000:SetCur;
	uint16_t modified_SetCur=(uint16_t)SetCur/1.19;
	TestVal=modified_SetCur;
	uint16_t val=(modified_SetCur/9600.0)*1.2/3.3*4096;
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
}
//Assuming a maximum expected current of 32.768A
//Uint micro_
void SCM_MonitorUpdate(void){
	SCM.CapVol=(int32_t)InaVol[1]*1.25;
	SCM.CapCur=(int32_t)InaCur[1];
	SCM.CapPow=(int32_t)InaPow[1]*25;
	SCM.BatVol=(int32_t)InaVol[0]*1.25;
	SCM.BatCur=(int32_t)InaCur[0];
	SCM.BatPow=(int32_t)InaPow[0]*25;

	DisplayVal[0]=SCM.CapVol;
	DisplayVal[1]=SCM.CapCur;
	DisplayVal[2]=SCM.CapPow;
	DisplayVal[3]=SCM.BatVol;
	DisplayVal[4]=SCM.BatCur;
	DisplayVal[5]=SCM.BatPow;

//	if(SCM.CapVol<Cap_MinVol)
//		SCM.EnergyPercent=0;
//	else
//		SCM.EnergyPercent=(uint16_t)1000*(SCM.CapVol*SCM.CapVol-Cap_MinVol*Cap_MinVol)/(Cap_MaxVol*Cap_MaxVol-Cap_MinVol*Cap_MinVol);
}
uint32_t PowerOf10(uint8_t m){
	uint32_t ret=1;
	if(m==0) return ret;
	while(m--) ret*=10;
	return ret;
}
void SCM_Display(void){
	strcpy(str[0],"CapVol=");
	strcpy(str[1],"CapCur=");
	strcpy(str[2],"CapPow=");
	strcpy(str[3],"BatVol=");
	strcpy(str[4],"BatCur=");
	strcpy(str[5],"BatPow=");

	for(int i=0;i<6;i++)
		if(DisplayVal[i]>=0)
			str[i][7]=' ';
		else{
			str[i][7]='-';
			DisplayVal[i]=-DisplayVal[i];
		}

	for(int i=0;i<6;i++)
		for(int j=0;j<DispLen;j++)
			str[i][7+DispLen-j]=(char)(DisplayVal[i]/PowerOf10(j)%10+'0');

	for(int i=0;i<6;i++)
		for(int j=8;j<8+DispLen-1;j++)
			if(str[i][j]=='0')
				str[i][j]=' ';
			else
				break;

	for(int i=0;i<6;i++)
		str[i][8+DispLen]='\0';
	//Display
	ssd1306_Fill(Black);
	for(int i=0;i<6;i++){
		 ssd1306_SetCursor(3, 10*i);
		 ssd1306_WriteString(str[i], Font_7x10, White);
	}
	ssd1306_UpdateScreen();
}








