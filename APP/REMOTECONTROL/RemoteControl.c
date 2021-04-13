/************************************************************
 *File		:	RemoteControl.c
  *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Remote data analysis
                Change remoteState (state of the system);
 ************************************************************/

#include "main.h"

RemoteState_e remoteState = PREPARE_STATE;
RemoteState_e remoteState_ex = PREPARE_STATE;

unsigned char auto_aim_flag=0x00;//���������־λ

unsigned char big_power_flag=0x00;//�����С����־λ
int16_t auto_aim_count=0;        //����������ʱ���
int16_t small_power_count=0;  //�����С����ʱ���
int now_can_count=0;
int last_can_count=0;
u8 view_flag_spec=0;

int bigbuff_leftright = 0;

int firstINAIMING = 0;
int temp=0;
extern int send_time_counter;
extern int entering_auto_aim;
/*-------------  ң����ѭ��  -------------*/
void RemoteControlLoop(void)
{
    ItoE();												//����Exֵ������Exֵ�����в���
    RemoteStateFSM();             //�жϴ�ʱң����״̬
    GimbalSwitchJudge();         //�ж�ң����ģʽ�л�
}

/*-------------  ң����ģʽ״̬��  -------------*/
void RemoteStateFSM(void)
{
    if(flag_Ready)
    {
        switch(RC_Ex_Ctl.rc.s2)       //�ж�s2ҡ��ֵ���ı�״̬
        {
        case 0:
            remoteState = ERROR_STATE;						//����ģʽ
            break;
        case 1:
            remoteState = NORMAL_REMOTE_STATE;  	//ң��������ģʽ
            break;
        case 2:
            remoteState = STANDBY_STATE;  				//����ģʽ
            break;
        case 3:
            remoteState = KEY_REMOTE_STATE;				//����ģʽ
            break;
        default:
            remoteState = PREPARE_STATE;          //��ʼ��׼��ģʽ
            break;
        }
        if(remoteState == NORMAL_REMOTE_STATE)
        {
//					 sendtoComputer(0,1,0,1);
			 if(send_time_counter%5==0)
			 {
				 sendtoComputer(0,1,0,entering_auto_aim);
				 if(entering_auto_aim)
					 entering_auto_aim=0;
			 }
//				auto_aim_flag = 0;
//
//			if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_Q ) == KEY_PRESSED_OFFSET_Q)
//				{
//					 robotLevel = 1;
//				}
//
//			if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_W ) == KEY_PRESSED_OFFSET_W)
//				{
//					 robotLevel = 2;
//				}
//
//			if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_E ) == KEY_PRESSED_OFFSET_E)
//				{
//					 robotLevel = 3;
//				}
//
            firstIN = 1;
            direction_wait = 1;
        }
        if(remoteState == KEY_REMOTE_STATE)
        {

            //�л�����ģʽ
            if((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_C ) == KEY_PRESSED_OFFSET_C)//||(RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_Z ) == KEY_PRESSED_OFFSET_Z 
            //if(1)
            {
                if(firstINAIMING == 0)
                {
                    YawTarget.Mechanical=continuous_current_position_205;
                    YawTarget.Gyroscope=Yaw*57.3f;
                    PitchTarget.Mechanical=continuous_current_position_206;
                    GimbalStop();
                    PIDOut_Whole_Yaw = 0;
                    PIDOut_Whole_Pit = 0;
                    auto_aim_yaw_temp = 0;
                    auto_aim_pit_temp = 0;
                    fram.yaw = 0;
                    fram.pitch = 0;

                    firstINAIMING = 1;
                }
                auto_aim_flag=0xFF;
            }
            else
            {
                auto_aim_flag=0x00;
                firstINAIMING = 0;
            }

						if(((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_Z ) == KEY_PRESSED_OFFSET_Z) || ((RC_Ex_Ctl.key.v & KEY_PRESSED_OFFSET_V ) == KEY_PRESSED_OFFSET_V)) //�л�С��
						//if(1)
						{
							big_power_flag = 0xFF;
						}
						else
						{
							big_power_flag = 0x00;
						}

            if( big_power_flag == 0xFF||auto_aim_flag==0xFF)
            {
                remoteState=VIEW_STATE;
            }

        }
        else
        {
            big_power_flag = 0;
            firstIN = 1;
            direction_wait = 1;

            if_rotate_ok = 0;
            why_not_shoot = 0;
            small_power_delay = 0;
        }

        if(auto_aim_count>0)
            auto_aim_count--;
        if(small_power_count>0)
            small_power_count--;
    }
}

/***************************************************************************************
 *Name     : GimbalSwitchJudge
 *Function ���жϻ���ģʽ�л�
 *Input    ����
 *Output   ����
 *Description : �жϻ����Ƿ��ǴӴ���ģʽ�����ģʽ�л�������ģʽ�����ǣ�������
****************************************************************************************/
void GimbalSwitchJudge(void)
{
    if((remoteState_ex == STANDBY_STATE || remoteState_ex == ERROR_STATE) && remoteState == NORMAL_REMOTE_STATE )
    {
        remoteState = PREPARE_STATE;    //ʹ���������ʼ��״̬����̨���ڳ�ʼ��ģʽ��������
        //flag_Ready = 0;                 //flag_Ready���㣬��֤�л�������ģʽʱ��̨������
        //GimbalPidInitPrepare();         //��̨��ʼ��pid���Խ����ٶȹ���
        remoteState_ex = NORMAL_REMOTE_STATE;
    }
    if((remoteState_ex == STANDBY_STATE || remoteState_ex == ERROR_STATE) && remoteState == KEY_REMOTE_STATE )
    {
        remoteState = PREPARE_STATE;    //ʹ���������ʼ��״̬����̨���ڳ�ʼ��ģʽ��������
        //flag_Ready = 0;                 //flag_Ready���㣬��֤�л�������ģʽʱ��̨������
        //GimbalPidInitPrepare();         //��̨��ʼ��pid���Խ����ٶȹ���
        remoteState_ex = KEY_REMOTE_STATE;
    }

}

/*-------------  ң������ʼ��  -------------*/
void RemoteControlInit(void)
{
    ItoE();
    remoteState = PREPARE_STATE;    //����2sǰ������ң�����źţ���������Զ���ֵΪPREPARE_STATEģʽ
}

