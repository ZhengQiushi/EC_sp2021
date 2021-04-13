 /************************************************************
 *File		:	monitorControl.c
 *Author	:	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.04.13
 *Description: 	Monitor of the whole system  (return 0 for no problem, 1 for error)
								including : 
								rc
								can
								adc
													
 ************************************************************/
#include "main.h"
#include "euler.h"
#define rc_monitor_time_limit 30 //ң������ؼ�ʱ����


int16_t rc_monitor_time = 0; //ң������ؼ�ʱ
float max_output_current=0;
static int u8count3 = 0;

extern double zf_result;

u8 AdcReceiveMonitor()     //��д
{
	return 0;
}



/*----------------------------------------------------
���Ƶ�����Χ		��-16384 ~ 16384	1
ת�ص�����Χ		��-20		 ~ 20			A
���̹�������		��60W
����						��Im = 60/24 = 2.5A
									Ic = 2.5x(16384/20) �� 2048
˥������ֵ			: 500

�ȼ���					��60W --- 2048/1200
									70W --- 4096/3000
									80W ---
-------------------------------------------------------*/


void Power_Moni(void)
{
	//max_output_current=5000-500*((float)Chassis_Power/(float)Power_Limitation_Num);//��ת�������ȷ������
	
#ifdef Referee_System
	
	max_output_current=max_output_current*ext_power_heat_data.chassis_power_buffer/60.0f;
	
//	u8count3++;
//	if(u8count3 == 50){
//		u8count3 = 0 ;
//		UART_DMA_SEND(ext_power_heat_data.chassis_power);
//					}
	
#endif
}

/*----------------------------------------------------
���ݵȼ����ù���������

�ȼ���					��level 1 ---- 60W 
									level 1 ---- 80W 
									level 1 ---- 100W 
-------------------------------------------------------*/
int Power_based_on_level(void){
	int a,b;
	a = ext_game_robot_state.robot_level;
	switch(a){
		case 1: b = 60000;break;
		case 2: b = 80000;break;
		case 3: b = 100000;break;

	}
	return b;
}

int max_output_speed_based_on_level(void){
	int a,b;
	a = ext_game_robot_state.robot_level;
	switch(a){
		case 1: b = 5400;break;
		case 2: b = 6400;break;
		case 3: b = 7000;break;

	}
	return b;
}

float heat_based_on_level(void){
	int a,b;
	a = ext_game_robot_state.robot_level;
	switch(a){
		case 1: b = 150;break;
		case 2: b = 280;break;
		case 3: b = 400;break;

	}
	return b;
}


u8 raging_press_delay=0;
u8 raging_mode=0;
void raging_monitor(void)
{
	
	if(raging_press_delay>0)
	{
		raging_press_delay--;
	}
	if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_B )==KEY_PRESSED_OFFSET_B&&raging_press_delay<=0)
	{
		if(raging_mode==0)
		{
			raging_mode=1;
		}
		else
		{
			raging_mode=0;
		}
		raging_press_delay=200;
	}
}





/*-------------  ң�������ӳ���  -------------*/
void RcReceiveMonitor()
{
		if(rc_monitor_count == rc_monitor_ex_count ) //����ֵû��
			{   
				if(rc_monitor_time > rc_monitor_time_limit)   //ң������һ��ʱ����û�仯 �ж�Ϊ����
					remoteState = ERROR_STATE;     
				else												//�����ۼ�
					rc_monitor_time++;
		  }
		else {                                       //ֵһֱ����
					rc_monitor_time  = 0;	
		  }
		rc_monitor_ex_count = rc_monitor_count;
}

u8 clearing_flag=0;
float yaw_position_0=0;
int clear_delay=0;
float dynamic_zero_float_offset=0;
	


void clear_zero_float(void)
{
	if(clear_delay>0)
	{
		clear_delay--;
	}
	

	
	if(remoteState==KEY_REMOTE_STATE)
	{
		if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_G )==KEY_PRESSED_OFFSET_G&&clear_delay<=0&&adi_die_flag==1)
		{
			yaw_position_0=continuous_current_position_205;
			clear_delay=1000;
			dynamic_zero_float_offset=0;
			clearing_flag=1;
		}
		else if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_G )==KEY_PRESSED_OFFSET_G&&clear_delay<=0&&adi_die_flag==0)
		{
		
			clear_delay=1000;
			aid_dynamic_mach_angle=0;
		}
		
		
		if(clear_delay==0&&clearing_flag==1)
		{
			dynamic_zero_float_offset=-(continuous_current_position_205-yaw_position_0)/1000.0f/8192.0f*2*PI;
			clearing_flag=0;
		}
	}
}

int can2_die_count=0;
void can2_monitor(void)
{
	if(current_position_207==0)
	{
		can2_die_count++;
	}
	if(can2_die_count>1000)
	{
		CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_6tq,5,CAN_Mode_Normal);
		can2_die_count=0;
	}
}



void Reset_monitor(void)
{
	if(remoteState == STANDBY_STATE)
	{
		if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_Q )==KEY_PRESSED_OFFSET_Q)
		{
			BSP_Init();	
		}
	}
}

int pre_guard_blood=200;
int now_guard_blood=200;
int show_count=0;
int send_count=0;
void Rescue_Moni(void)
{
#ifdef BULE_TEAM
	now_guard_blood=ext_game_robot_survivors.blue_7_robot_HP
#endif
	
#ifdef RED_TEAM
	now_guard_blood=ext_game_robot_survivors.red_7_robot_HP
#endif	
	
	if(show_count>0)
	{
		show_count--;
	}
	//(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_F )==KEY_PRESSED_OFFSET_F
	//pre_guard_blood!=now_guard_blood
	if(pre_guard_blood!=now_guard_blood)
	{
		show_count=3;
		Send_SOS();
	}
	
	if(show_count==0)
	{
		Clear_SOS();
		show_count=-1;
	}
	
	pre_guard_blood=now_guard_blood;
}

int send_rect_count=0;
void press_send_rectangle(void)
{
	if(send_rect_count>0)
	{
		send_rect_count--;
	}
	
	
	if(remoteState == STANDBY_STATE)
	{
		if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_R )==KEY_PRESSED_OFFSET_R&&send_rect_count<=0)
		{
			Send_Middle_rectangle(2,5,23,18);
			send_rect_count=2000;
		}
		
		if(send_rect_count==1000)
		{
			Send_Middle_rectangle(3,2,900,750);
		}
	}
}




/*************************************************
Function		: monitorControlLoop
Description	: rcReceive
							247ms:Printf some information

*************************************************/
int monitor_loop_count=0;
double hand_zero_clear=ZERO_FLOAT_INIT;
double hand_zero_clear_pre=ZERO_FLOAT_INIT;
int cleaning_flag=0;
int cleaning_countdown=0;
int hand_cleaning_sent=0;
void monitorControlLoop(void)						//1000ms per loop 
{
	RcReceiveMonitor();
	raging_monitor();
	Power_Moni();
	clear_zero_float();
	can2_monitor();
	Reset_monitor();
	press_send_rectangle();
	if(cleaning_countdown>0)
	{
		cleaning_countdown--;
	}
	
	
//	if(monitor_loop_count<1000)
//	{
//		monitor_loop_count++;
//	}
//	else
	{
		if(rc_monitor_time<30&&(RC_Ex_Ctl.rc.s2==0||RC_Ex_Ctl.rc.s1==0))
			{	
				RC_Init();
				monitor_loop_count=0;
			}
	}
	if(remoteState != VIEW_STATE)
	{
		timeout_count=1001;
	}
#ifdef INFANTRY_Jiangsu
	if(remoteState == KEY_REMOTE_STATE)
	{
	
		if(RC_Ex_Ctl.rc.ch1>600)
		{
				Buzzer_toggle();
			cleaning_flag=1;
			cleaning_countdown=10000;
		}
		if(RC_Ex_Ctl.rc.ch1<-600||cleaning_countdown<=0)
		{
			cleaning_flag=0;
		}
		
		
		if(cleaning_flag==1)
		{
			zf_result+=RC_Ex_Ctl.rc.ch0*0.0000001;
			if(hand_zero_clear!=hand_zero_clear_pre)
			{
				CAN2_Send_Clear((int)hand_zero_clear);
				hand_cleaning_sent=1;
			}
			hand_zero_clear_pre=hand_zero_clear;
		}
	}
#endif
	
}
