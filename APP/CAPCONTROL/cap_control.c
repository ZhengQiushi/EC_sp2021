#include "main.h"


int32_t Power_Limitation_Num=60000;
int32_t Residue_Power;

int8_t CAN2_Cap_flag=0;
int8_t Cap_Switch_Flag=0;

int32_t Lost_Connection_Count=0;
uint8_t Cap_Switch_Count=0;

int32_t Cap_Toutuous_Uppest=25000;
int32_t Cap_Toutuous_Up=24000;
int32_t Cap_Toutuous_Down=20000;

int32_t Chassis_Power=0;



void CapContorl(void)
{
	
Lost_Connection_Count+=1;
if(Lost_Connection_Count>60000)Lost_Connection_Count=60001;

#ifdef Cap_Offline_Debug

int8_t Debug_Flag=0;
	
	
if(Debug_Flag==0 && CAN2_Cap_flag!=4) //放电
	{
		if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)//回环判断
			{
				CAN2_Cap_flag=2;
			}
		else if (CAN2_Cap_flag==2 && SCM_rx_message.cap_vol<Cap_Toutuous_Uppest)
			{
				Residue_Power=Power_Limitation_Num/25;
				CAN2_Cap_Transmes(2);//边充边放
			}
		else
			{
				CAN2_Cap_Transmes(3);//纯电容放电
				CAN2_Cap_flag=0;
			}
		if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)
			{
				CAN2_Cap_flag=4;
			}
	}
else
	{
		if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)//回环判断
			{
				CAN2_Cap_flag=1;
			}
		if(CAN2_Cap_flag==1 && SCM_rx_message.cap_vol<Cap_Toutuous_Uppest && SCM_rx_message.chassis_power<Power_Limitation_Num)
			{
				Residue_Power=(Power_Limitation_Num-SCM_rx_message.chassis_power)/25;
				CAN2_Cap_Transmes(1);//边充边放
			}
		else
			{
				CAN2_Cap_Transmes(0);//纯电池放电
				CAN2_Cap_flag=0;
			}
	}

#else
	
#ifdef Referee_System

	Chassis_Power=ext_power_heat_data.chassis_power*1000;		
	//Power_Limitation_Num = Power_based_on_level();
	
		
		
#else
	if(Lost_Connection_Count<100)
		{
			Chassis_Power=SCM_rx_message.chassis_power;
		}
	else
		{
			Chassis_Power=current_sum*24;
		}
	
		
#endif
	if(remoteState == KEY_REMOTE_STATE||remoteState == NORMAL_REMOTE_STATE||remoteState == VIEW_STATE)
		{
			if(Cap_Switch_Flag==1)
			{
				Cap_Switch_Count+=1;
				if(Cap_Switch_Count<10)
					CAN2_Cap_Transmes(3);
				else if(Cap_Switch_Count<50)
					CAN2_Cap_Transmes(0);
				else 
				{
				if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)//检测电容要不要充电
						CAN2_Cap_flag=1;
				if(CAN2_Cap_flag==1 && SCM_rx_message.cap_vol<(Cap_Toutuous_Uppest+500))	
						CAN2_Cap_Transmes(1);
				else
				{
					CAN2_Cap_flag=0;
					Cap_Switch_Flag=0;
					Cap_Switch_Count=0;
				}
				}
			}
			else
			{
			if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_F ) == KEY_PRESSED_OFFSET_F ) //电容测试键位
				{
					if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)
						{
							CAN2_Cap_flag=2;
						}
					if (CAN2_Cap_flag==2 && SCM_rx_message.cap_vol<Cap_Toutuous_Uppest)
						{
							CAN2_Cap_Transmes(3);
						}
					else
						{
							CAN2_Cap_Transmes(3);
							CAN2_Cap_flag=0;
						}
					if(SCM_rx_message.cap_vol<Cap_Toutuous_Down)
						{
							Cap_Switch_Flag=1;
						}
					}
			else
			{

				if(SCM_rx_message.cap_vol<Cap_Toutuous_Uppest+100)//检测电容要不要充电
					{
						CAN2_Cap_flag=1;
					}
				if(CAN2_Cap_flag==1 && SCM_rx_message.cap_vol<(Cap_Toutuous_Uppest+1000))	
					{
						CAN2_Cap_Transmes(1);
					}
				else
					{
						CAN2_Cap_Transmes(0);
						CAN2_Cap_flag=0;
					}
			}
		}
			
		
		}
	else if(remoteState == STANDBY_STATE)
		{
			if(SCM_rx_message.cap_vol<Cap_Toutuous_Up)//检测电容要不要充电
				{
					CAN2_Cap_flag=1;
				}
			
			if(CAN2_Cap_flag==1 && SCM_rx_message.cap_vol<Cap_Toutuous_Uppest && Chassis_Power<Power_Limitation_Num)		
				{
					CAN2_Cap_Transmes(1);
				}
			else
				{
					CAN2_Cap_Transmes(0);
					CAN2_Cap_flag=0;
				}
		}
	else
		{
  			CAN2_Cap_Transmes(0);
		}

#endif
}
