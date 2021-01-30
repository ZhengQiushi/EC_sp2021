#include "main.h"

PID_Type FrictionPID_l,FrictionPID_r;
float Friction_p_l=5;//0.05;
float Friction_i_l=0;
float Friction_d_l=0.2;
	
float Friction_p_r=5;//0.05;
float Friction_i_r=0;
float Friction_d_r=0.2;

int16_t frictionState;
u8 flag_FrictionDelay = 0;



int16_t Friction_speed_target_l=0;
int16_t Friction_speed_target_r=0;
int PIDOut_friction_l=0;
int PIDOut_friction_r=0;
int speed_level=0;

int shoot_speed_change_flag_count=0;
int speed_switch_flag=0;
int speed_switch_flag_count=0;

void FrictionControlInit(void)
{
	PidFrictionInit();	

	
}

void PidFrictionInit(void)
{
		PID_ControllerInit(&FrictionPID_l,20,20,16384,0.002);
		PID_SetGains(&FrictionPID_l,Friction_p_l,Friction_i_l,Friction_d_l);
		PID_ControllerInit(&FrictionPID_r,20,20,16384,0.002);
		PID_SetGains(&FrictionPID_r,Friction_p_r,Friction_i_r,Friction_d_r);
}

void Friction_Para_Moni(void)
{
	if(remoteState == KEY_REMOTE_STATE)
	{
			 if(shoot_speed_change_flag_count>0)
			 {
				 speed_switch_flag_count--;
			 }
	     if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_CTRL )==KEY_PRESSED_OFFSET_CTRL&&shoot_speed_change_flag_count<=0)
	     {
		     if(speed_switch_flag==0)
				 {
			      speed_switch_flag=1;
				 }
		     else
				 {
					 speed_switch_flag=0;
				 }
		        speed_switch_flag_count=200;
				 
			 }
			
	}	
}
	

void FrictionControlLoop(void)
{

  Friction_Para_Moni();
  FrictionJudge();

 if((remoteState == NORMAL_REMOTE_STATE)||(remoteState == KEY_REMOTE_STATE))
  {
    if(frictionState == Friction_OFF)
      {
         Friction_speed_target_l=0;
				 Friction_speed_target_r=0;
      }
    else if(frictionState == Friction_ON)
      {
				if (speed_level==0)Friction_speed_target_l=0;
        else if(speed_level==1)Friction_speed_target_l=-7500;
        else if(speed_level==2)Friction_speed_target_l=-7500;
      }
	}	
  else if ((remoteState == PREPARE_STATE)||(remoteState == STANDBY_STATE) ||(remoteState == ERROR_STATE))
	{
	     Friction_speed_target_l=0;
		   speed_level=0;
	}
  
    Cover_Moni();

		Friction_speed_target_r=-Friction_speed_target_l;

		PIDOut_friction_l=PID_ControllerDriver(&FrictionPID_l, Friction_speed_target_l, current_fric_s_201);
		PIDOut_friction_r=PID_ControllerDriver(&FrictionPID_r, Friction_speed_target_r, current_fric_s_202);

		CAN1_fri_Cmd(PIDOut_friction_l,PIDOut_friction_r);


}
void FrictionJudge(void)
{
	if((remoteState == NORMAL_REMOTE_STATE)||(remoteState == KEY_REMOTE_STATE))
	{
	if(frictionState == Friction_OFF && flag_FrictionDelay == 0 ) 
	 {                        
		 speed_level=0;
		if(flag_friction_switch == 1)                     
			{
				frictionState = Friction_ON;
				flag_friction_switch=0;
			}   
	 }
	else if(frictionState == Friction_ON)
	 {
		 if(speed_switch_flag==1)
		 {speed_level=2;}
		 else if(speed_switch_flag==0)
		 {speed_level=1;}
		if( flag_friction_switch == 1)
			{
				frictionState = Friction_OFF;
				flag_friction_switch = 0;
			}
	 }

	 if(flag_FrictionDelay > 0) {flag_FrictionDelay--;}  
 }
 else	if((remoteState == PREPARE_STATE)||(remoteState == STANDBY_STATE) ||(remoteState == ERROR_STATE))
 { 
	 frictionState = Friction_OFF;
 }
}




void Cover_Moni(void)
{
	if(RC_Ex_Ctl.rc.s1==2)
	{
		TIM_SetCompare1(TIM9, COVER_OPEN);
	}
	else
	{
		TIM_SetCompare1(TIM9, COVER_CLOSE);

	}
}