 /*
  ******************************************************************************
  * @author         : Crazt
  * @brief          : Super capacitor module control program
  * 				  For Tongji SuperPower 2020
  ******************************************************************************

  ******************************************************************************
 */
#include "stm32f1xx.h"

typedef struct{
	uint8_t Buck;   //Charge module status run/off
	uint8_t Mode;   //Power path select  from battery or capacitor
	uint8_t ForceUseCap;

	int32_t SetPow; //Set Capacitor charging power
	int32_t SetCur; //Set Capacitor charging current (Don't edit this value)

	//Below are value collected from power monitor
	int32_t CapVol;
	int32_t CapCur;
	int32_t CapPow;

	int32_t BatVol;
	int32_t BatCur;
	int32_t BatPow;

	uint16_t CAN_ConnectWDT;
} SCM_StatusTypeDef;

extern SCM_StatusTypeDef SCM;

typedef struct{
	uint8_t Buck;   //Charge module status run/off
	uint8_t Mode;   //Power path select  from battery or capacitor
	uint8_t ForceUseCap;

	int32_t SetPow; //Set Capacitor charging power
	int32_t SetCur; //Set Capacitor charging current
} SCM_NextStatus_TypeDef;

extern SCM_NextStatus_TypeDef SCM_Next;

typedef struct{
	int32_t Cap_MaxVol_H;
	int32_t Cap_MaxVol_L;
	int32_t Cap_MinVol_H;
	int32_t Cap_MinVol_L;
}SCM_Settings_TypeDef;

extern SCM_Settings_TypeDef SCM_Settings,SCM_SettingsNext;

extern uint8_t SCM_CanReceiveFlag;
#define SCM_CAN_Ready ((uint8_t)0x00)
#define SCM_CAN_Busy  ((uint8_t)0x01)

#define SCM_Buck_Off ((uint8_t)0x00)
#define SCM_Buck_Run ((uint8_t)0x01)

#define SCM_Mode_Bat ((uint8_t)0x00)
#define SCM_Mode_Cap ((uint8_t)0x01)

#define Cap_MaxVol 26200  //Maxwell:26.5V
#define Cap_MinVol 18000  //18V

//hysteresis comparator parameter
#define Cap_MaxVolLow  25500  //Maxwell 25.6V
#define Cap_MinVolHigh 21000  //21V

#define Cap_Capacity 5    //5F

void SCM_Init(void);
void SCM_ControlLoop(void);
void SCM_SetCurrent(int32_t SetCur);
void SCM_SetPower(int32_t SetPow);
void SCM_MonitorUpdate(void);
void SCM_Display(void);

