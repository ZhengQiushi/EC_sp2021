/****************************************************************************************************
 *File		:	CMControl.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Chassis Motors.
								CMControlLoop() shows the way to control the motion of chassis in different states.
                Use PID to optimize Chassis motor control.
 *****************************************************************************************************/

#include "main.h"

int16_t CMFollowVal=0; 			    //���̸���ֵ
int16_t speedA_final,speedB_final,speedC_final,speedD_final;
//PID_Struct CMPid,CMFollowPid,CMFollowPid_speed;                 //�����˶�pid������pid
float step=100,keymove_x=0,keymove_y=0,m=0,n=0,q=0;           //step-�ٶȱ仯��  x-x�᷽��仯ֵ y-y�ٶȱ仯ֵ;
float followwatch;

u8 quick_spin_flag=0;
int16_t CMFollowVal_QUICK=0;


PID_Type SPFOLLOW,SPCHIASSISA,SPCHIASSISB,SPCHIASSISC,SPCHIASSISD,SPFOLLOW_SPEED;


float max_output_speed=5500;//60W ��������20j����
//float max_output_speed=6500;//80W ��������20J
//float max_output_speed=7000;//100W ��������30J

float singl_max=4000;
float speed_step=500;
float output_current_sum=0;

float spin_y=0;
float spin_x=0;

u8 climb_mode_flag=0;

float cm_normal_p1=0.9;//��¼��0.9;

float cm_normal_p=5.0;//8.0;
float cm_normal_i=3.2;//5.2
float cm_normal_d=0;

float cm_climb_p=8.5;
float cm_climb_i=20;
float cm_climb_d=0.0;

float cm_normal_follow_p=0.29;
float cm_normal_follow_i=0.32;
float cm_normal_follow_d=0.004;

float cm_climb_follow_p=0.23;
float cm_climb_follow_i=0.3;
float cm_climb_follow_d=0.002;

float rotate_speed=250;
int rotate_speed_dir=1;
float rotate_change_para=1;
int rotate_counter=0;
int rotate_counter_100ms=0;
/*-------------  ���̿���ѭ��  -------------*/
void CMControlLoop(void)
{

    CM_Switch_Moni();
    if(remoteState == PREPARE_STATE)
    {
        CMControlOut(0,0,0,0);
    }
    else if(remoteState == NORMAL_REMOTE_STATE)
    {
        CMFollowVal = followValCal(0);
			  CM_Normal_PID1();
        move(RC_Ex_Ctl.rc.ch0,RC_Ex_Ctl.rc.ch1,CMFollowVal/*0.5f*(RC_Ex_Ctl.rc.ch2)*/);
    }
    else if(remoteState == STANDBY_STATE )
    {
        CMStop();
    }
    else if(remoteState == ERROR_STATE )
    {
        CMStop();
    }
    else if(remoteState == KEY_REMOTE_STATE )
    {
				if(rotate_counter<10000)
				{
					rotate_counter++;
				}
				else
				{
					rotate_counter=0;
				}
				if(rotate_counter_100ms<1000)
				{
					if(rotate_counter%100==0)
					{
						rotate_counter_100ms++;
					}
				}
				else
				{
					rotate_counter_100ms=0;
				}
				
				
        if(clearing_flag==0)
        {
            if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT)
            {
                CM_Normal_PID();
                keyboardmove(RC_Ex_Ctl.key.v,1500,1500);//С�����ƶ���
            }
            else if(climb_mode_flag==1)
            {
                CM_Climb_PID();
                keyboardmove(RC_Ex_Ctl.key.v,2000,2500);//�������µ�λ
            }
            else
            {
                CM_Normal_PID();
                keyboardmove(RC_Ex_Ctl.key.v,8000,8000);//�����˶���
            }

            //С����ȫ��ת��
            if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_W )!=KEY_PRESSED_OFFSET_W
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_A )!=KEY_PRESSED_OFFSET_A
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_S )!=KEY_PRESSED_OFFSET_S
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_D )!=KEY_PRESSED_OFFSET_D)
            {
								rotate_change_para=sin(rotate_counter_100ms);//���� 
                CMFollowVal=(rotate_speed+20/**fabs(rotate_change_para)*/)*rotate_speed_dir;//����300��Ϊ55
                key_move(keymove_x,keymove_y,CMFollowVal);
            }
            //С������ת�ƶ���
            else if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT)
            {
                CMFollowVal=rotate_speed*rotate_speed_dir;
                float theta=position_yaw_relative/8192.0f*2*PI;
                spin_x=keymove_x*cos(theta)-keymove_y*sin(theta);
                spin_y=keymove_x*sin(theta)+keymove_y*cos(theta);
                key_move(spin_x,spin_y,CMFollowVal);
            }
            //�����˶���
						else if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_CTRL )==KEY_PRESSED_OFFSET_CTRL)      //����
						{
							 
							   CMFollowVal=0;
								//CMFollowVal = followValCal(1000);
								
                key_move(keymove_x,keymove_y+2000,CMFollowVal);
               // YawPID_MechanicalAngle(1600);
							
						}
            else
            {
//								rotate_change_para=sin(rotate_counter);
                CMFollowVal = followValCal(0);
                key_move(keymove_x,keymove_y,CMFollowVal);
							
            }
        }
        else
        {
            CMStop();
        }
    }
    else if(remoteState == VIEW_STATE )
    {
        if(auto_aim_flag==0xFF)
        {
            if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT)
            {
                CM_Normal_PID();
                keyboardmove(RC_Ex_Ctl.key.v,1500,1500);//С�����ƶ���
            }
            else if(climb_mode_flag==1)
            {
                CM_Climb_PID();
                keyboardmove(RC_Ex_Ctl.key.v,2000,2500);//�������µ�λ
            }
            else
            {
                CM_Normal_PID();
                keyboardmove(RC_Ex_Ctl.key.v,8000,8000);//�����˶���
            }

            //С����ȫ��ת��
            if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_W )!=KEY_PRESSED_OFFSET_W
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_A )!=KEY_PRESSED_OFFSET_A
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_S )!=KEY_PRESSED_OFFSET_S
                    &&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_D )!=KEY_PRESSED_OFFSET_D)
            {
                CMFollowVal=(rotate_speed+55)*rotate_speed_dir;
                key_move(keymove_x,keymove_y,CMFollowVal);
            }
            //С������ת�ƶ���
            else if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT)
            {
                CMFollowVal=rotate_speed*rotate_speed_dir;
                float theta=position_yaw_relative/8192.0f*2*PI;
                spin_x=keymove_x*cos(theta)-keymove_y*sin(theta);
                spin_y=keymove_x*sin(theta)+keymove_y*cos(theta);
                key_move(spin_x,spin_y,CMFollowVal);
            }
            //�����˶���
            else
            {
                CMFollowVal = followValCal(0);
                key_move(keymove_x,keymove_y,CMFollowVal);
            }
        }
        else
        {
            CMStop();
        }
    }


}




/*-------------  ����ֹͣ  -------------*/
void CMStop(void)
{
    CAN2_Cmd_Bottom(0,0,0,0);
}


/***************************************************************************************
 *Name     : move
 *Function ��������̵�  ���ٶȸ���ֵ ��ң�������ơ�ͬʱ�и������ӵ��ٶ�����
 *Input    ��speedX, speedY, rad
 *Output   ����
 *Description :�ı���ȫ�ֱ���	speedA, speedB, speedC, speedD
****************************************************************************************/
void move(int16_t speedX, int16_t speedY, int16_t rad)
{
    speedX *= gears_speedXYZ;
    speedY *= gears_speedXYZ;
    rad *= gears_speedRAD;


//		if(fabs(speedY)>6000&&fabs(rad)<100)
//		{
//			rad*=10;
//		}

    int16_t speedA = ( speedX + speedY + rad);
    int16_t speedB = ( speedX - speedY + rad);
    int16_t speedC = (-speedX - speedY + rad);
    int16_t speedD = (-speedX + speedY + rad);


    CMControlOut(speedA,speedB,speedC,speedD);
}


void key_move(int16_t speedX, int16_t speedY, int16_t rad)
{
    float max_speed=0;
    rad *= gears_speedRAD;

    if(abs(speedY)>5000&&abs(rad)<50&&abs(rad)>5)
    {
        rad*=10;
    }
		
    int16_t speedA = ( speedX + speedY + rad);
    int16_t speedB = ( speedX - speedY + rad);
    int16_t speedC = (-speedX - speedY+ rad);
    int16_t speedD = (-speedX + speedY + rad);

    
    CMControlOut(speedA,speedB,speedC,speedD);
}



/***************************************************************************************
 *Name     : CMControlOut
 *Function �����̵���ٶ����������PID�����������͸������������ͬʱ�и������ӵ��ٶ�����
 *Input    ��speedA,speedB,speedC,speedD
 *Output   ����
 *Description : ���̵���ٶȻ��ͷ�ֵ����
****************************************************************************************/
int16_t out_a=0;
float lec_numA=3000;
float lec_numB=3000;
float lec_numC=3000;
float lec_numD=3000;
static int u8count2 = 0;

float speda1 = 0;
float speda2 = 0;



int16_t pre_current_A=0;
int16_t pre_current_B=0;
int16_t pre_current_C=0;
int16_t pre_current_D=0;
int16_t current_step =800;//600;
u8 super_cap_flag=0;
float w_danger=45;
void CMControlOut(int16_t spa , int16_t spb ,int16_t spc ,int16_t spd )
{

		float max_speed=0;
    float current_sum=0;
    int max_now_speed=0;
		float k=0.1;
		max_output_speed= max_output_speed_based_on_level();//���ݵȼ��仯���� ��ò�����ǵ�ʹ��
/* --------------SPEED CTRL---------------*/
	if(abs(spa)>max_speed)
        max_speed=abs(spa);
    if(abs(spb)>max_speed)
        max_speed=abs(spb);
    if(abs(spc)>max_speed)
        max_speed=abs(spc);
    if(abs(spd)>max_speed)
        max_speed=abs(spd);

    if(max_speed>max_output_speed)
    {
        spa*=max_output_speed/max_speed;
        spb*=max_output_speed/max_speed;
        spc*=max_output_speed/max_speed;
        spd*=max_output_speed/max_speed;
    }
/* --------------SPEED CTRL---------------*/

		
    float speedA = PID_ControllerDriver(&SPCHIASSISA,spa,current_cm_201);
    float speedB = PID_ControllerDriver(&SPCHIASSISB,spb,current_cm_202);
    float speedC = PID_ControllerDriver(&SPCHIASSISC,spc,current_cm_203);
    float speedD = PID_ControllerDriver(&SPCHIASSISD,spd,current_cm_204);

//    if(cap_receive.state_module==Cap_Run&&cap_receive.state_cap==Cap_Boost&&cap_receive.CapVol>11800)
//    //if(1)
//    {
//        CAN2_Cmd_Bottom(speedA,speedB,speedC,speedD);
//        super_cap_flag=0;
//    }
//    else
	
/* --------------control the step of valicity---------------*/
			
        if(fabs(speedA-pre_current_A)>current_step)
        {
            if(speedA-pre_current_A>current_step)
                speedA=pre_current_A+current_step;
            else if(speedA-pre_current_A<-current_step)
                speedA=pre_current_A-current_step;
        }
        if(fabs(speedB-pre_current_B)>current_step)
        {
            if(speedB-pre_current_B>current_step)
                speedB=pre_current_B+current_step;
            else if(speedB-pre_current_B<-current_step)
                speedB=pre_current_B-current_step;
        }
        if(fabs(speedC-pre_current_C)>current_step)
        {
            if(speedC-pre_current_C>current_step)
                speedC=pre_current_C+current_step;
            else if(speedC-pre_current_C<-current_step)
                speedC=pre_current_C-current_step;
        }
        if(fabs(speedD-pre_current_D)>current_step)
        {
            if(speedD-pre_current_D>current_step)
                speedD=pre_current_D+current_step;
            else if(speedD-pre_current_D<-current_step)
                speedD=pre_current_D-current_step;
        }
				
				
	
/* --------------control the step of valicity---------------*/

#ifdef Power_Limitation
		Chassis_Power=ext_power_heat_data.chassis_power*1000;
				
//����Σ��ֵ���ö�̬Power_Limitation_Num
//		if(ext_power_heat_data.chassis_power_buffer > w_danger)
//			//Power_Limitation_Num = 60000+((ext_power_heat_data.chassis_power_buffer - w_danger)/0.01)*1000;		//0.01�ǹ���̽�����ڣ�����ϵͳ
//		Power_Limitation_Num = Power_based_on_level()+((ext_power_heat_data.chassis_power_buffer - w_danger)/0.01)*1000;//���ݵȼ��仯���� ��ò�����ǵ�ʹ��
//	 else
//		 //Power_Limitation_Num = 60000;
			Power_Limitation_Num = Power_based_on_level();//���ݵȼ��仯���� ��ò�����ǵ�ʹ��
    if(Chassis_Power < Power_Limitation_Num)//������޳�����	
	{
		Residue_Power=Power_Limitation_Num-Chassis_Power;
		if(Residue_Power>5000)Residue_Power-=5000;
		Residue_Power/=25;
			
		CAN2_Cmd_Bottom(speedA,speedB,speedC,speedD);
	}
	
	else	//������Ҫ���������
    {
		Residue_Power=0;        

				
				
		if(Chassis_Power > Power_Limitation_Num)
        {
						speedA*=k*(float)Power_Limitation_Num/((float)Chassis_Power);
            speedB*=k*(float)Power_Limitation_Num/((float)Chassis_Power);
            speedC*=k*(float)Power_Limitation_Num/((float)Chassis_Power);
            speedD*=k*(float)Power_Limitation_Num/((float)Chassis_Power);
        }
		
				
				
				
				
        CAN2_Cmd_Bottom(speedA,speedB,speedC,speedD);
	}
		
#else 
	  Residue_Power=Power_Limitation_Num-Chassis_Power;

		if(Residue_Power>5000)Residue_Power-=5000;
		Residue_Power/=25;
		CAN2_Cmd_Bottom(speedA,speedB,speedC,speedD);
#endif
	u8count2++;
				
    output_current_sum=fabs(speedA)+fabs(speedB)+fabs(speedC)+fabs(speedD);
	if(u8count2 == 10){
					u8count2 = 0 ;
					UART_DMA_SEND(ext_power_heat_data.chassis_power_buffer);
					//ext_power_heat_data.chassis_power_buffer;
					}

    pre_current_A=speedA;
    pre_current_B=speedB;
    pre_current_C=speedC;
    pre_current_D=speedD;


}


/**********  ���̵����ֵ����  **********/
float CMSpeedLegalize(float MotorCurrent , float limit)
{
    return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : followValCal
 *Function �����̵���������
 *Input    ��Setposition,Yaw�������̸���ֵ
 *Output   �����̸�����
 *Description : ���̵�����������㺯��
****************************************************************************************/
int16_t followValCal(float Setposition)
{
    int16_t followVal = 0;
    float NowPosition = position_yaw_relative;

    followVal=PID_ControllerDriver(&SPFOLLOW,Setposition,NowPosition);
//	float followVal_speed=PID_ControllerDriver(&SPFOLLOW_SPEED,followVal/20.0,yaw_speed);
//	followVal = CMSpeedLegalize(followVal_speed,1200);
    //��������Сֵ���Ƕȹ�С������
		followwatch=followVal;
    if(abs(followVal) < followVal_limit) 
			followVal = 0;
		if (fabs(NowPosition)<10)
			followVal = 0;
    return followVal;
}



/*-------------  ���̵���ٶ�PID�͸���PID��ʼ��  -------------*/



/*���̳�ʼ��*/
void CMControlInit(void)
{
    PID_ControllerInit(&SPFOLLOW,50,150,1100,0.01);
    SPFOLLOW.intergration_separation = 100;
		//(p;i;d;dt)
    PID_ControllerInit(&SPCHIASSISA,20,20,16384,0.01);	//20,20,16384,0.01 -> A,B,C,D
    PID_ControllerInit(&SPCHIASSISB,20,20,16384,0.01);
    PID_ControllerInit(&SPCHIASSISC,20,20,16384,0.01);
    PID_ControllerInit(&SPCHIASSISD,20,20,16384,0.01);
	
    PID_ControllerInit(&SPFOLLOW_SPEED,20,20,1500,0.01);
    PID_SetGains(&SPFOLLOW, 0.29,0.3,0.004);// 0.28,0.3,0.8
    PID_SetGains(&SPFOLLOW_SPEED,3,0.5,0.1);
    CM_Normal_PID();
}
//Ϊ�˹�20sײǽ��¼��pƫС��
void CM_Normal_PID1(void)
{
    PID_SetGains(&SPCHIASSISA,cm_normal_p1,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISB,cm_normal_p1,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISC,cm_normal_p1,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISD,cm_normal_p1,cm_normal_i,cm_normal_d);
		PID_SetGains(&SPFOLLOW,cm_normal_follow_p,cm_normal_follow_i,cm_normal_follow_d);// 0.28,0.3,0.8
}

void CM_Normal_PID(void)
{
    PID_SetGains(&SPCHIASSISA,cm_normal_p,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISB,cm_normal_p,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISC,cm_normal_p,cm_normal_i,cm_normal_d);
    PID_SetGains(&SPCHIASSISD,cm_normal_p,cm_normal_i,cm_normal_d);
		PID_SetGains(&SPFOLLOW,cm_normal_follow_p,cm_normal_follow_i,cm_normal_follow_d);// 0.28,0.3,0.8
}

void CM_Climb_PID(void)
{
    PID_SetGains(&SPCHIASSISA,cm_climb_p,cm_climb_i,cm_climb_d);
    PID_SetGains(&SPCHIASSISB,cm_climb_p,cm_climb_i,cm_climb_d);
    PID_SetGains(&SPCHIASSISC,cm_climb_p,cm_climb_i,cm_climb_d);
    PID_SetGains(&SPCHIASSISD,cm_climb_p,cm_climb_i,cm_climb_d);
		PID_SetGains(&SPFOLLOW,cm_climb_follow_p,cm_climb_follow_i,cm_climb_follow_d);// 0.28,0.3,0.8
}


void keyboardmove(uint16_t keyboardvalue,uint16_t xlimit,uint16_t ylimit)
{

    // W and S
    switch(keyboardvalue & (KEY_PRESSED_OFFSET_W|KEY_PRESSED_OFFSET_S))
    {
    case ( KEY_PRESSED_OFFSET_W):
        m = 0;
        if(keymove_y>0)
            keymove_y += 0.8f*step;
        else
            keymove_y += step*1.6;
        break;
    case ( KEY_PRESSED_OFFSET_S):
        m = 0;
        if(keymove_y<0)
            keymove_y -= 0.8f*step;
        else
            keymove_y -= step*1.6;
        break;
    default:
        m++;
        if(m>1)
        {
            if(keymove_y>1.8*step) {
                keymove_y=keymove_y-1.8*step;
            }
            else if(keymove_y<-1.8*step) {
                keymove_y=keymove_y+1.8*step;
            }
            else {
                keymove_y = 0;
            }
        }
        break;
    }

    //  A and D  stand for X axis
    switch(keyboardvalue & (KEY_PRESSED_OFFSET_A | KEY_PRESSED_OFFSET_D))
    {
    case ( KEY_PRESSED_OFFSET_A):
        n = 0;
        if(keymove_x<0)
            keymove_x -= 0.8*step;//ϵ��Ϊ3
        else
            keymove_x -= 0.8*step;
        break;
    case ( KEY_PRESSED_OFFSET_D):
        n = 0;
        if(keymove_x>0)    //��������
            keymove_x += 0.8*step;           //�Ӳ���
        else
            keymove_x += 0.8*step;
        break;

    default:
    n++;

    if(n>1)
    {
            if(keymove_x>1.2*step) {
                keymove_x=keymove_x-1.2*step;
            }
            else if(keymove_x<-1.2*step) {
                keymove_x=keymove_x+1.2*step;
            }
            else {
                keymove_x = 0;
            }
        }
        break;
    }
    keymove_x = keymove_x>xlimit?xlimit:(keymove_x<-xlimit?-xlimit:keymove_x);  //����
    keymove_y = keymove_y>ylimit?ylimit:(keymove_y<-ylimit?-ylimit:keymove_y);

    if(keymove_x<5&&keymove_x>-5) keymove_x = 0;
    if(keymove_y<5&&keymove_y>-5) keymove_y = 0;	 //��������

}

u8 cm_switch_delay=0;


int rotate_shift_count=0;
void CM_Switch_Moni(void)
{
    if(cm_switch_delay>0)
        cm_switch_delay--;


    if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_X )==KEY_PRESSED_OFFSET_X&&cm_switch_delay<=0)
    {
        if(climb_mode_flag==1)
            climb_mode_flag=0;
        else
            climb_mode_flag=1;
        cm_switch_delay=50;
    }

    if(	rotate_shift_count>-1)
    {
        rotate_shift_count--;
    }

    if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )==KEY_PRESSED_OFFSET_SHIFT)
    {
        rotate_shift_count=50;
    }
    if(rotate_shift_count==0&&(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_SHIFT )!=KEY_PRESSED_OFFSET_SHIFT)
    {
        rotate_speed_dir=-rotate_speed_dir;
    }


}








