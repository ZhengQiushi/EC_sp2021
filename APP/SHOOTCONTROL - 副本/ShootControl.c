/************************************************************
 *File		:	ShootControl.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Application for Shoot Motor, including 2 friction motor and 1 common DC motor.
								ShootControlLoop() shows the main control for Shoot motors.
								Pay attention to FrictionControl()
 ************************************************************/

#include "main.h"

#ifndef Referee_System
int robotLevel=0;//�޲���ϵͳʱ����������1-3�ȼ���������Ϊ0��
#else
int robotLevel=1;//�ֶ��޸ĵ�ǰ�Զ�������˵ȼ�
#endif

#define Friction_OFF 0
#define Friction_ON  1
#define friction_count_time 40


int Shoot_UP=100;       //Ħ���ֿ�
#define Shoot_DOWN 0     //Ħ���ֹ�	

#define ShootInterval_0 25
#define ShootInterval_1 22
#define ShootInterval_2 12
#define ShootTime_0 22       //���̵��ת��ʱ��
#define ShootTime_1 18       
#define ShootTime_2 10       
#define ShootSpeedOffset_1 1900  //���̵��ת��   10��47��
//#define ShootSpeedOffset_1 2500								//10��60��
#define ShootSpeedOffset_2 3500
//#define ShootSpeedOffset_2 3000									//10��70��
#define ShootSpeedOffset_3 5000	//          10��114�� 


#define Friction_SPEED_p               0.06f
#define Friction_SPEED_i               0.005f
#define Friction_SPEED_d               0.01f
#define Friction_INTE_limitI           60.f
#define DELTA_TIME                     0.01f


PID_Type	MotorShoot;	


int16_t frictionState;
int16_t friction_count ;
u8 flag_FrtctionDelay = 0;
u8 flag_trigger = 0;			//�жϲ�������Ƿ�ִ��
static uint32_t t1_5ms = 0;	//��������ļ��ʱ��
static uint32_t t2_5ms = 0;	//�����תPID��ֵʱ��  t2<t1   һֱ��
static int gun_count = 0;          //���ڰ�������
float PIDOut_Speed_Shoot,PIDOut_Whole_Shoot;					//Pit,Yaw PID�����
float shoot_speed_set = 0;
int shoot_temp = 1;
int friction_acce_flag=0;
int acce_delay=30;
int acce_delay_init=30;
int friction_speed_temp=Shoot_DOWN;

int shooterHeat0_limit_0 = 50;
int shooterHeat0_limit_1 = 160;
int shooterHeat0_limit_2 = 340;

int shoot_flag = 0;

uint32_t shootTime_tick_1ms =0;
PWMFriction_Type    Friction_CH1;
PWMFriction_Type    Friction_CH2;
/***************************************************************************************
 *Name     : ShootControlLoop
 *Function ��Ħ���ֺͲ��̵������ 
 *Input    ����
 *Output   ���� 
 *Description : ���ݻ���״̬�ж�Ħ���ֺͲ��̵�����Ʒ���
****************************************************************************************/
void ShootControlLoop(void)
{
	if(remoteState == PREPARE_STATE)
	{
		ShootStop();

	}
	else if(remoteState == NORMAL_REMOTE_STATE)
	{
		FrictionJudge();					//�ж�Ħ�����Ƿ�Ӧ�ô�
		GunControl();
		
		
		if(friction_acce_flag==1)
		{
			acce_delay--;
		}
		if(friction_acce_flag==1&&acce_delay<=0&&friction_speed_temp<Shoot_UP)
		{
			friction_speed_temp+=5;//����
			acce_delay=acce_delay_init;
			Friction_Looper(friction_speed_temp);
		}
		else if (friction_acce_flag==1&&acce_delay<=0&&friction_speed_temp>=Shoot_UP)
		{
			friction_acce_flag=0;
		}
				if(RC_Ctl.rc.s1==2)
		{
			TIM_SetCompare3(TIM4,520);
		}
		else
		{
			TIM_SetCompare3(TIM4,930);
		}
		shoot_cal(shoot_speed_set);
		
	}
	else if(remoteState == KEY_REMOTE_STATE )  
	{
		FrictionJudge();
		GunControl();
		
				if(friction_acce_flag==1)
		{
			acce_delay--;
		}
		if(friction_acce_flag==1&&acce_delay<=0&&friction_speed_temp<Shoot_UP)
		{
			friction_speed_temp+=5;//����
			acce_delay=acce_delay_init;
			Friction_Looper(friction_speed_temp);
		}
		else if (friction_acce_flag==1&&acce_delay<=0&&friction_speed_temp>=Shoot_UP)
		{
			friction_acce_flag=0;
		}
		
		
		
		if(RC_Ctl.rc.s1==2)
		{
			TIM_SetCompare3(TIM4,520);
		}
		else
		{
			TIM_SetCompare3(TIM4,930);
		}
		shoot_cal(shoot_speed_set);
	}
	else if(remoteState == VIEW_STATE)
	{
		FrictionJudge();					//�ж�Ħ�����Ƿ�Ӧ�ô�
		GunControl();
	}
	else if(remoteState == STANDBY_STATE ) 
	{
		ShootStop();
	}
	else if(remoteState == ERROR_STATE ) 
	{
		ShootStop();
	}
	
}
int stuck_count=0;
int inver_rotate_delay=0;
int inver_rotate_delay_init=20;

void shoot_cal(float shoot_speed_set )
{
	
	if(shoot_speed_set!=0&&current_shoot_speed_207<3)
	{
		stuck_count++;
	}
	if(stuck_count>100)
	{
		inver_rotate_delay=inver_rotate_delay_init;
		stuck_count=0;
	}
	
	if(inver_rotate_delay>0)
	{
		inver_rotate_delay--;
		PIDOut_Whole_Shoot=PID_ControllerDriver(&MotorShoot,-shoot_speed_set,current_shoot_speed_207);
	}
	else
	{
		PIDOut_Whole_Shoot=PID_ControllerDriver(&MotorShoot,shoot_speed_set,current_shoot_speed_207);
	}
}

/***************************************************************************************
 *Name     : FrictionJudge
 *Function ���ж�Ħ�����Ƿ�Ӧ�ô�
 *Input    ����
 *Output   ���� 
 *Description : ����Ҽ�����������ҡ��s1��3��1λ���ٴ�1��3λ���л�һ�ο���
****************************************************************************************/
void FrictionJudge(void)
{
	
	//�ж�Ħ���ֿ�����ر�
	if(frictionState == Friction_OFF && flag_FrtctionDelay == 0 )  //ֻ�е�flag_FrtctionDelayΪ0ʱ�����ж�Ħ����״̬
	 {                                                                                      
		if(flag_friction_switch == 1)                     //flag_friction_switch��rc.c�и�ֵ����s1�л�����flag_friction_switch=1
      {
			  frictionState = Friction_ON;
			  flag_friction_switch=0;
				FrictionControl();
			}   
	 }
	else if(frictionState == Friction_ON)
	 {
		if( flag_friction_switch == 1)
      {
				frictionState = Friction_OFF;
				flag_friction_switch = 0;
				FrictionControl();
			}
	 }
	
	//�������β����������ֹ��������
	if(flag_FrtctionDelay > 0) flag_FrtctionDelay--;    
}

/*-------------  Ħ����ʵ�ֺ���  -------------*/
void FrictionControl(void)
{
	if(frictionState == Friction_OFF )
	{
		Friction_Looper(Shoot_DOWN);
		PIDOut_Whole_Shoot = 0;
		friction_acce_flag=0;
		friction_speed_temp=Shoot_DOWN;
		LASER_OFF();
//		LASER_ON();
		
	}
	if(frictionState == Friction_ON)
	{
			friction_acce_flag=1;
			LASER_ON();
	}
}

/*-------------  ֹͣ�������  -------------*/
void ShootStop(void)				
{
	Friction_Looper(Shoot_DOWN);
	friction_speed_temp=Shoot_DOWN;
	shoot_speed_set = 0;
	PIDOut_Whole_Shoot = 0;
	frictionState = Friction_OFF;
}

/*-------------  �����������  -------------*/
void GunControl(void)			
{
	if(frictionState == Friction_OFF)
	{
		ShootStop();
	}
	if(frictionState == Friction_ON)	   //ֻ����Ħ���ֿ���������£����������������
	{
		GunShootAction();   
	}
}

/*-------------  ���������������  -------------*/
void GunShootAction(void)						
{
	if(RC_Ex_Ctl.mouse.press_l == 1 || RC_Ex_Ctl.mouse.press_r == 1 || (RC_Ex_Ctl.rc.s1 == 2 && RC_Ex_Ctl.rc.s2==1)) gun_count++;
	else gun_count = 0;
	
	if(t1_5ms > 0)	t1_5ms--;	          //��������ļ��ʱ����� 	
	
	
	if(t1_5ms == 0 && gun_count > 0)	  //t1_5ms Ϊ0ʱ��ҡ��s1����1λ�û����������£���������ӵ�
	 {		 
		 
		 if(ext_game_robot_state.robot_level == 1)
		 {
			t1_5ms = ShootInterval_0;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime_0; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset_1;	
		 }
		  if(ext_game_robot_state.robot_level == 2)
		 {
			t1_5ms = ShootInterval_1;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime_1; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset_2;	
		 }
		 if(ext_game_robot_state.robot_level == 3)
		 {
				t1_5ms = ShootInterval_2;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
				t2_5ms = ShootTime_2; 	          //������Կ�ת
			  shoot_speed_set = ShootSpeedOffset_3;
			
		 }
		 
		 
		  if(robotLevel == 1)
		 {
			t1_5ms = ShootInterval_0;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime_0; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset_1;	
		 }
		  if(robotLevel == 2)
		 {
			t1_5ms = ShootInterval_1;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime_1; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset_2;	
		 }
		 if(robotLevel == 3)
		 {
			t1_5ms = ShootInterval_2;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime_2; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset_3;

			
		 }
		 
		 
		 
		 if(RC_Ex_Ctl.mouse.press_r == 1)
		 {
			t1_5ms = ShootInterval_2;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
		  t2_5ms = ShootTime_2; 
			 
			
			 
			 	if(ext_game_robot_state.robot_level == 1 )
					shoot_speed_set = 1900;	
				if(ext_game_robot_state.robot_level == 2 )
					shoot_speed_set = 3800;
				if(ext_game_robot_state.robot_level == 3 )
				{
					shoot_speed_set = 5500;
          Shoot_UP=100;
					friction_acce_flag=1;
				}	
				
				if(robotLevel == 1 )
					shoot_speed_set = 1900;	
				if(robotLevel == 2 )
					shoot_speed_set = 3800;	
				if(robotLevel == 3 )
				{
					shoot_speed_set = 5500;
          Shoot_UP=100;
					friction_acce_flag=1;
				}					
		 }
		 else{
			 Shoot_UP=100;
			 friction_acce_flag=1;
		 }

	 }
	
	if(t2_5ms > 0)                      //���������t2_5ms ���Ʋ��̵��ת��ʱ��
	 {
		 t2_5ms--;	                      //�����תʱ�䣬t2<t1��һֱ�� 
	 }
	else
	 {
		shoot_speed_set = 0;
	 }
	

	 if(robotLevel == 1 && ext_power_heat_data.shooter_heat0 > shooterHeat0_limit_0)
		 shoot_speed_set = 0;	 
	 if(robotLevel == 2 && ext_power_heat_data.shooter_heat0 > shooterHeat0_limit_1)
		 shoot_speed_set = 0;
	 if(robotLevel == 3 && ext_power_heat_data.shooter_heat0 > shooterHeat0_limit_2)
		 shoot_speed_set = 0;
}

void PidShootInit(void)
{
	PID_ControllerInit(&MotorShoot,5,50,9000,0.002);
	PID_SetGains(&MotorShoot,8,0.8,5);
}
void ShootControlInit(void)
{
	PidShootInit();	
	//���������pwmƵ��Ϊ50HZ��������2ms���������������ޣ����Ϊ2ms����3s��������ͣת���ޣ����Ϊ1ms����ÿ�ο�����Ҫ���á�
	Friction_Looper(Shoot_DOWN);
	//delay_ms(1000);
	ShootStop();
}

void Friction_Init(void) {
    TIM8_Init();
 
    
    PID_ControllerInit(&Friction_CH1.pid, Friction_INTE_limitI, (uint16_t)-1, 160, DELTA_TIME);
    Friction_CH1.pid.Kp = Friction_SPEED_p;
    Friction_CH1.pid.Ki = Friction_SPEED_i;
    Friction_CH1.pid.Kd = Friction_SPEED_d;
    Friction_CH1.pid.intergration_separation = 20.f;
    Friction_CH1.pid.functions.output_filter = MovingAverageFilter_f32;
    
    PID_ControllerInit(&Friction_CH2.pid, Friction_INTE_limitI, (uint16_t)-1, 160, DELTA_TIME);
    Friction_CH2.pid.Kp = Friction_SPEED_p;
    Friction_CH2.pid.Ki = Friction_SPEED_i;
    Friction_CH2.pid.Kd = Friction_SPEED_d;
    Friction_CH2.pid.intergration_separation = 20.f;
    Friction_CH2.pid.functions.output_filter = MovingAverageFilter_f32;
}

/***************************************************************************************
Name:Friction_Looper
Function:Ħ����ִ�к���
***************************************************************************************/
 uint32_t check = 0;
int16_t temp_pwm=0;
void Friction_Looper(uint32_t target) {
		temp_pwm=800-target;
    TIM_SetCompare1(TIM8, temp_pwm);
    TIM_SetCompare2(TIM8, temp_pwm);
		TIM_SetCompare3(TIM8, check);
    TIM_SetCompare4(TIM8, check);
}


