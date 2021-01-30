#include "main.h"

#define Feed_OFF 0
#define Feed_ON  1


/*PID parameter*/
PID_Type Feed_Speed_PID;
PID_Type Feed_Position_PID;

float Feed_PID_SP=12;//40.0;
float Feed_PID_SI=0.1;//3.2;
float Feed_PID_SD=0;//1;

float Feed_PID_PP=50;//40.0;
float Feed_PID_PI=0;//3.2;
float Feed_PID_PD=0;//1;

int Feed_speed_target;
int Feed_position_target;
float PIDOut_Position_Shoot,PIDOut_Speed_Shoot,PIDOut_Whole_Shoot;
int PIDOut_Feed;

int Feed_Mode;  //-1,stuck 0,feed_stop  1,clear_bomb 2,triple tap 3,signal shoot 4,reach the heat limit under triple tap
int Mode1_stop_flag = 0;

/*triple singal switch parameter*/
int Triple_Tap = 3;
int Signal_Shoot = 1;
int tri_sin_swi_flag = 0; //0 triple tap   1 signal shoot
int press_R_count = 0;
int mode_change_flag = 0;
int mode_change_delay = 0;
int mode_change_delay_init = 100;
/*stuck_parameter*/
int stuck_count=0;
int inver_rotate_delay=0;
int inver_rotate_delay_init=20;
/*RefereeInfo_parameter*/
float heat_limite=360;
int remain_bullet=0;


extern int16_t frictionState;

void FeedControlInit(void)
{
  PidFeedInit();
	
}

void StuckMoni(void)
{
	if((fabs(current_speed_207)<1&&PIDOut_Feed>8000))
	{
		stuck_count++;
	}
	if(stuck_count>300)    //0.6s
	{
		inver_rotate_delay=inver_rotate_delay_init;
		stuck_count=0;
	}
	
}
void StuckMoni_Speed(void)
{
	if((Feed_speed_target>100&&fabs(current_speed_207<5)))
	{
		stuck_count++;
	}
	if(stuck_count>300)     //0.6s
	{
		inver_rotate_delay=inver_rotate_delay_init;
		stuck_count=0;
	}
}

void FeedJudge()
{
  if(remoteState == NORMAL_REMOTE_STATE&&inver_rotate_delay<=0)
	{
		if(frictionState==Friction_ON&&RC_Ex_Ctl.rc.s1==2)
		{
		  
			Feed_Mode = 1;
		}
		else
		{
			Feed_Mode = 0;
		}			
	}
	else if((remoteState == KEY_REMOTE_STATE||remoteState ==VIEW_STATE)&&inver_rotate_delay<=0)
	{
		if(frictionState==Friction_ON)
		{
			if(remain_bullet<=1)
			{
				Feed_Mode = 0;
			}
			else
			{
				if(RC_Ctl.mouse.press_r)//清弹
				{
						Feed_Mode = 1;
					  Mode1_stop_flag = 1;
				}
				else if((RC_Ctl.mouse.press_r==0)&&(Mode1_stop_flag == 1))
				{
					Feed_Mode = 0;
					Mode1_stop_flag = 0;
				}
				else if((RC_Ctl.mouse.press_l)&&(tri_sin_swi_flag==0))//三连发
				{
					if(remain_bullet>3)
					{
						Feed_Mode = 2;
					}
					else 
					{
						Feed_Mode = 4;
					}
					}
				else if((RC_Ctl.mouse.press_l)&&(tri_sin_swi_flag==1))//单发
				{
						Feed_Mode = 3;
				}
				
			}
		}
	}
	else if(inver_rotate_delay>0)
	{
		Feed_Mode=-1;
		inver_rotate_delay--;
	}
	
}

void PidFeedInit(void)
{
	PID_ControllerInit(&Feed_Speed_PID,50,50,9800,0.01);
	PID_SetGains(&Feed_Speed_PID,Feed_PID_SP,Feed_PID_SI,Feed_PID_SD);
	PID_ControllerInit(&Feed_Position_PID,50,50,9800,0.01);
	PID_SetGains(&Feed_Position_PID,Feed_PID_PP,Feed_PID_PI,Feed_PID_PD);
}

void TargetRenew(u8 flag)
{
	switch(flag)
	{
		case 0:
		{
			Feed_position_target=continuous_current_position_207;
			Feed_speed_target=current_speed_207;
		}break;
		case 1:
		{
			Feed_position_target=continuous_current_position_207;
		}break;
		case 2:
		{
			Feed_speed_target=current_speed_207;
		}break;
	}
}

void Shoot_Para_Moni(void)
{
	  if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_R )==KEY_PRESSED_OFFSET_R)
	  {
				press_R_count++;
				if(mode_change_flag==0)
				{
					tri_sin_swi_flag = 0;
					mode_change_flag = 1;
					mode_change_delay = mode_change_delay_init;
				}
			  
				if(press_R_count>=300)
				{ 
					tri_sin_swi_flag = 1;
					press_R_count = 0;
				}	
		}
		else{
				if(mode_change_flag==1)
				{
					mode_change_delay--;
				}
				if(mode_change_delay<=0)
				{
					mode_change_flag=0;
					press_R_count=0;
				} 
		}
		
		
		remain_bullet=(heat_limite-ext_power_heat_data.shooter_heat0)/25.0f;
}

void Shoot_target_Cal(void)
{
  if(Feed_Mode==-1)
	{Feed_speed_target=-3000; PIDOut_Feed=PID_ControllerDriver(&Feed_Speed_PID,Feed_speed_target,current_speed_207);}
	else if(Feed_Mode==0)
	{Feed_speed_target=0;PIDOut_Feed=PID_ControllerDriver(&Feed_Speed_PID,Feed_speed_target,current_speed_207);}
	else if(Feed_Mode==1)
	{Feed_speed_target=-3300;PIDOut_Feed=PID_ControllerDriver(&Feed_Speed_PID,Feed_speed_target,current_speed_207);StuckMoni_Speed();}
	else if(Feed_Mode==2)
	{if(RC_Ctl.mouse.press_l){Feed_position_target=-1*(continuous_current_position_207+Triple_Tap*30);}ShootMove(Feed_position_target);StuckMoni();}
	else if(Feed_Mode==3)
	{if(RC_Ctl.mouse.press_l){Feed_position_target=-1*(continuous_current_position_207+Signal_Shoot*40);}ShootMove(Feed_position_target);StuckMoni();}
	else if(Feed_Mode==4)
	{if(RC_Ctl.mouse.press_l){Feed_position_target=-1*(continuous_current_position_207+remain_bullet*40);}ShootMove(Feed_position_target);StuckMoni();}
}


void ShootMove(int SetPos) 
{
	float NowPos = continuous_current_position_207;
	float NowSpeed=current_speed_207;
	

  PIDOut_Position_Shoot=PID_ControllerDriver(&Feed_Position_PID,SetPos,NowPos);
	PIDOut_Speed_Shoot=PID_ControllerDriver(&Feed_Speed_PID,PIDOut_Position_Shoot,NowSpeed);
	PIDOut_Feed = PIDOut_Speed_Shoot;	
}


void FeedControlLoop(void)
{
	Shoot_Para_Moni();
	FeedJudge();
	Shoot_target_Cal();
	CAN1_Feed_Cmd(PIDOut_Feed);
}


