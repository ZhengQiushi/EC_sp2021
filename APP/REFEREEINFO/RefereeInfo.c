/******************
������Ϣ�����͸������֡��װ����
update: 2017.5.7
        ��������������������
        ȫ�ֱ���˵����RefereeInfo.h
        ֧���ϴ�3��float����
******************/
#include "main.h"
#include <stdio.h>
#include <string.h>
//#include "Task.h"

u16 cmd_id;  // ���ݰ�ID
// ������Ϣ��ؽṹ��  
ext_game_state_t                           ext_game_state;// ����״̬���ݣ�0x0001��
ext_game_result_t                          ext_game_result;//�����������(0x0002)
ext_game_robot_survivors_t                 ext_game_robot_survivors;//�����˴������ݣ�0x0003��
ext_event_data_t                           ext_event_data;//����ʱ�¼����ݣ�0x0101��
ext_supply_projectile_action_t             ext_supply_projectile_action;//����վ������ʶ��0x0102��
ext_supply_projectile_booking_t            ext_supply_projectile_booking;//����վԤԼ�ӵ���0x0103��
ext_game_robot_state_t                     ext_game_robot_state;//����������״̬(0x0201)
ext_power_heat_data_t                      ext_power_heat_data;////ʵʱ�����������ݣ�0x0202��
ext_game_robot_pos_t                       ext_game_robot_pos;//������λ�ã�0x0203��
ext_buff_musk_t                            ext_buff_musk;//���������棨0x0204��
aerial_robot_energy_t                      ext_aerial_robot_energy;//���л���������״̬��0x0205��
ext_robot_hurt_t                           ext_robot_hurt;//�˺�״̬��0x0206��
ext_shoot_data_t                           ext_shoot_data;//ʵʱ�����Ϣ��0x0207��

ext_student_interactive_header_data_t      ext_student_interactive_header_data;//�������ݽ�����Ϣ��0x0301��
client_custom_data_t                       client_custom_data;
//robot_interactive_data_t                   robot_interactive_data;

const u8 sof=0xA5;  // ֡ͷ
u8 custom_info_counter=0;  // �Զ������ݰ����
u8 seq = 0;  // �������İ����
u8 seq_real=0;

Bytes2Float bytes2float;  // flaot���ֽڻ�ת
Bytes2U32 bytes2u32;  // flaot��u16��ת

u8 referee_message[64];  // ��������֡���, ����44�͹���
int message_byte_counter = 0;  // �ֽڴ��λ�ü���
u8 blood_counter=0;  // (debug)�������

int	shoot_seq = 0;//shoot number


uint32_t last_time_tick_1ms_1 = 0;
uint32_t time_interval_1=0;

void init_referee_info() {
	ext_game_robot_pos.x = 0;
	ext_game_robot_pos.y = 0;
	ext_game_robot_pos.z = 0;
	ext_game_robot_pos.yaw = 0;
	ext_power_heat_data.shooter_heat0 = 0;
	ext_power_heat_data.shooter_heat1= 0;
}

//crc8 ���ɶ���ʽ:G(x)=x8+x5+x4+1
const unsigned char CRC8_INIT2 = 0xff;
const unsigned char CRC8_TAB[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};


//crc16 ���ɶ���ʽ: δ֪
uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/*
** Descriptions: CRC8 У��������
** Input: ��Ϣ����, ��Ϣ���ݳ�, CRC8_INIT2(0xff)
** Output: CRC8У����
*/
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8) {
    unsigned char ucIndex;
    while(dwLength--) {
        ucIndex = ucCRC8^(*pchMessage++);
        ucCRC8 = CRC8_TAB[ucIndex];
    }
    return(ucCRC8);
}


/*
** Descriptions: CRC8 У�����
** Input: ��У������, ���ݳ���(= ��Ϣ���� + У���볤��)
** Output: True or False (CRC Verify Result)
*/
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength) {
    unsigned char ucExpected = 0;
    if ((pchMessage == 0) || (dwLength <= 2)) 
        return false;
    ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT2);
    return (ucExpected == pchMessage[dwLength-1]);  // ĩλУ�����ж�
}


/*
** Descriptions: ���� CRC8 ����Ϣ����β��
** Input: ��Ϣ����(β��������1λ�Լ���У����), ��Ϣ���ݳ���
** Output: ��У���������
*/
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength) {
    unsigned char ucCRC = 0;
    if ((pchMessage == 0) || (dwLength <= 2))
        return;
    ucCRC = Get_CRC8_Check_Sum((unsigned char *)pchMessage, dwLength-1, CRC8_INIT2);  // У��������
    pchMessage[dwLength-1] = ucCRC;  // ������β��
}


/*
** Descriptions: CRC16 У��������
** Input: ��Ϣ����, ��Ϣ���ݳ�, CRC_INIT(0xffff)
** Output: CRC16У����
*/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
    uint8_t chData;
    if (pchMessage == NULL) {
        return 0xFFFF;
    }
    while(dwLength--) {
        chData = *pchMessage++;
        (wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
    }
    return wCRC;
}


/*
** Descriptions: CRC16 У�����
** Input: ��У������, ���ݳ���(= ��Ϣ���� + У���볤��, ��������У����ĳ���)
** Output: True or False (CRC Verify Result)
*/
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength) {
    uint16_t wExpected = 0;
    if ((pchMessage == NULL) || (dwLength <= 2)) {
        return false;
    }
    wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
    return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}


/*
** Descriptions: ���� CRC16 ����Ϣ����β��
** Input: ��Ϣ����(β��������2λ�Լ���У����), ��Ϣ���ݳ���
** Output: ��У���������
*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength) {
    uint16_t wCRC = 0;
    if ((pchMessage == NULL) || (dwLength <= 2)) {
        return;
    }
    wCRC = Get_CRC16_Check_Sum ( (u8 *)pchMessage, dwLength-2, CRC_INIT );
    pchMessage[dwLength-2] = (u8)(wCRC & 0x00ff);
    pchMessage[dwLength-1] = (u8)((wCRC >> 8)& 0x00ff);
}


///////////////////



// ���ֽ�����תu16(2�ֽ�), �ߵ�λ����
uint16_t _bytes2u16(uint8_t * chosen_Message) {
    uint32_t temp=0;
    // ����Ϊ2���ߵ�λ����ƴ��
    temp = *chosen_Message;
    chosen_Message++;
    temp += (*chosen_Message<<8);
    return temp;
}


// ���ֽ�����תu32(4�ֽ�), �ߵ�λδ֪
uint32_t _bytes4u32(uint8_t * chosen_Message) {
    bytes2u32.b[0] = chosen_Message[0];
    bytes2u32.b[1] = chosen_Message[1];
    bytes2u32.b[2] = chosen_Message[2];
    bytes2u32.b[3] = chosen_Message[3];
    return bytes2u32.u32_value;
}


// ���ֽ�����תfloat(4�ֽ�), �ߵ�λ����
float _bytes2float(uint8_t * chosen_Message) {
    bytes2float.b[0] = chosen_Message[0];
    bytes2float.b[1] = chosen_Message[1];
    bytes2float.b[2] = chosen_Message[2];
    bytes2float.b[3] = chosen_Message[3];
    return bytes2float.f;
}


// floatת4���ȵ��ֽ�����
void float2bytes(float chosen_value, u8 * res_message) {
    int i;
    bytes2float.f = chosen_value;
    for(i=0;i<4;i++)
        res_message[i] = bytes2float.b[i];
}




// ����������״̬��0x0001��, ����Ƶ��Ϊ10Hz��
void ext_game_state_interpret(uint8_t * ext_game_state_Message)
{
	uint8_t *a;
	memcpy(a,ext_game_state_Message,1);
	ext_game_state.game_type=(*a)>>4;
	ext_game_state.game_progress=(*a)&0x000f;
	
	memcpy((uint8_t*)&ext_game_state.stage_remain_time,ext_game_state_Message+1,1);
}

//�����������(0x0002)
void  ext_game_result_interpret(uint8_t * ext_game_result_t_Message)
{
	ext_game_result.winner=*ext_game_result_t_Message;

}

//�����˴������ݣ�0x0003��
void ext_game_robot_survivors_interpret(uint8_t * ext_game_robot_survivors_t_Message)
{
	//memcpy((uint8_t*)&ext_game_robot_survivors.robot_legion,ext_game_robot_survivors_t_Message,2);
	memcpy((uint8_t*)&ext_game_robot_survivors,ext_game_robot_survivors_t_Message,27);
}

//����ʱ�¼����ݣ�0x0101��...........�������д�����̫��
void ext_event_data_interpret(uint8_t * ext_event_data_t_Message)
{
	memcpy((uint8_t*)&ext_event_data.event_type,ext_event_data_t_Message,4);
}

//����վ������ʶ��0x0102��
void ext_supply_projectile_action_interpret(uint8_t * ext_supply_projectile_action_Message)
{
	memcpy((uint8_t*)&ext_supply_projectile_action.supply_projectile_id,ext_supply_projectile_action_Message,1);
	memcpy((uint8_t*)&ext_supply_projectile_action.supply_robot_id,ext_supply_projectile_action_Message+1,1);
	memcpy((uint8_t*)&ext_supply_projectile_action.supply_projectile_step,ext_supply_projectile_action_Message+2,1);
}

//����վԤԼ�ӵ���0x0103��
void ext_supply_projectile_booking_interpret(uint8_t * ext_supply_projectile_booking_Message)
{
	memcpy((uint8_t*)&ext_supply_projectile_booking.supply_projectile_id,ext_supply_projectile_booking_Message,1);
	memcpy((uint8_t*)&ext_supply_projectile_booking.supply_num,ext_supply_projectile_booking_Message+1,1);
}

//����������״̬(0x0201)
void ext_game_robot_state_interpret(uint8_t * ext_game_robot_state_Message)
{
	memcpy((uint8_t*)&ext_game_robot_state.robot_id,ext_game_robot_state_Message,1);
	memcpy((uint8_t*)&ext_game_robot_state.robot_level,ext_game_robot_state_Message+1,1);
	memcpy((uint8_t*)&ext_game_robot_state.remain_HP,ext_game_robot_state_Message+2,2);
	memcpy((uint8_t*)&ext_game_robot_state.max_HP,ext_game_robot_state_Message+4,2);
	memcpy((uint8_t*)&ext_game_robot_state.shooter_heat0_cooling_rate,ext_game_robot_state_Message+6,2);//17mmǹ��ÿ����ȴֵ
	memcpy((uint8_t*)&ext_game_robot_state.shooter_heat0_cooling_limit,ext_game_robot_state_Message+8,2);//17mm��������
	memcpy((uint8_t*)&ext_game_robot_state.shooter_heat1_cooling_rate,ext_game_robot_state_Message+10,2);//42mm��ȴ
	memcpy((uint8_t*)&ext_game_robot_state.shooter_heat1_cooling_limit,ext_game_robot_state_Message+12,2);//42mm��������
	
	uint8_t a;
	memcpy(&a,ext_game_robot_state_Message+14,1);/////////////////
	
	ext_game_robot_state.mains_power_gimbal_output=a>>7;
	ext_game_robot_state.mains_power_chassis_output=(a>>6)&0x0001;
	ext_game_robot_state.mains_power_shooter_output=(a>>5)&0x0001;
	
}

///////ʵʱ�����������ݣ�0x0202��
void ext_power_heat_data_interpret(uint8_t * ext_power_heat_data_Message)
{
	
	memcpy((uint8_t*)&ext_power_heat_data.chassis_volt,ext_power_heat_data_Message,2);
	memcpy((uint8_t*)&ext_power_heat_data.chassis_current,ext_power_heat_data_Message+2,2);
	memcpy((float*)&ext_power_heat_data.chassis_power,ext_power_heat_data_Message+4,4);
	// float temp;
	//ext_power_heat_data.chassis_power=_bytes2float(ext_power_heat_data_Message+4);
	//ext_power_heat_data.chassis_power_buffer=_bytes2float(ext_power_heat_data_Message+8);
memcpy((uint8_t*)&ext_power_heat_data.chassis_power_buffer,(ext_power_heat_data_Message+8),2);
	memcpy((uint8_t*)&ext_power_heat_data.shooter_heat0,ext_power_heat_data_Message+10,2);
	memcpy((uint8_t*)&ext_power_heat_data.shooter_heat1,ext_power_heat_data_Message+12,2);
	
}


////������λ�ã�0x0203��
void ext_game_robot_pos_interpret(uint8_t * ext_game_robot_pos_Message)
{
	memcpy((uint8_t*)&ext_game_robot_pos.x,ext_game_robot_pos_Message,4);
	memcpy((uint8_t*)&ext_game_robot_pos.y,ext_game_robot_pos_Message+4,4);
	memcpy((uint8_t*)&ext_game_robot_pos.z,ext_game_robot_pos_Message+8,4);
	memcpy((uint8_t*)&ext_game_robot_pos.yaw,ext_game_robot_pos_Message+12,4);
	
}

////���������棨0x0204��
void ext_buff_musk_interpret(uint8_t * ext_buff_musk_Message)
{
	memcpy((uint8_t*)&ext_buff_musk.power_rune_buff,ext_buff_musk_Message,1);
	
}

//���л���������״̬��0x0205��
void aerial_robot_energy_interpret(uint8_t * aerial_robot_energy_Message)
{
	memcpy((uint8_t*)&ext_aerial_robot_energy.energy_point,aerial_robot_energy_Message,1);
	memcpy((uint8_t*)&ext_aerial_robot_energy.attack_time,aerial_robot_energy_Message+1,2);
}

////�˺�״̬��0x0206��
void  ext_robot_hurt_interpret(uint8_t *  ext_robot_hurt_Message)
{
	uint8_t a;
	memcpy((uint8_t*)&a,ext_robot_hurt_Message,1);
	
	ext_robot_hurt.armor_id=a>>4;
	ext_robot_hurt.hurt_type=a&0x000f;
}

////ʵʱ�����Ϣ��0x0207��
void ext_shoot_data_interpret(uint8_t * ext_shoot_data_Message)
{
	memcpy((uint8_t*)&ext_shoot_data.bullet_type,ext_shoot_data_Message,1);
	memcpy((uint8_t*)&ext_shoot_data.bullet_freq,ext_shoot_data_Message+1,1);
	memcpy((uint8_t*)&ext_shoot_data.bullet_speed,ext_shoot_data_Message+2,4);  //�ӵ�����
	
	shoot_seq++;
}

////�������ݽ�����Ϣ��0x0301��
void ext_student_interactive_header_data_interpret(uint8_t * ext_student_interactive_header_data_Message)
{
	memcpy((uint8_t*)&ext_student_interactive_header_data.data_cmd_id,ext_student_interactive_header_data_Message,2);
	memcpy((uint8_t*)&ext_student_interactive_header_data.send_ID,ext_student_interactive_header_data_Message+2,2);
	memcpy((uint8_t*)&ext_student_interactive_header_data.receiver_ID,ext_student_interactive_header_data_Message+4,2);
	memcpy((uint8_t*)&ext_student_interactive_header_data.blood_value,ext_student_interactive_header_data_Message+6,2);
}
//////////////////////////////////////////////////
//�ͻ����Զ������ݣ�0x0301��������ID��data_cmd(0xD180)
void client_custom_data_interpret(uint8_t * client_custom_data_Message)
{
	memcpy((uint8_t*)&client_custom_data.data1,client_custom_data_Message+6,4);
	memcpy((uint8_t*)&client_custom_data.data2,client_custom_data_Message+10,4);
	memcpy((uint8_t*)&client_custom_data.data3,client_custom_data_Message+14,4);
	memcpy((uint8_t*)&client_custom_data.masks,client_custom_data_Message+18,1);
	
}

//void robot_interactive_data_interpret(uint8_t * robot_interactive_data_Message)
//{
	//memcpy(&robot_interactive_data.data,robot_interactive_data_Message,sizeof(data));
	
//}

// ����У������֡, CRC8��CRC16
u8 Verify_frame(uint8_t * frame) {
    int frame_length;
	if(frame[0]!= sof) return false;
    frame_length = _bytes2u16(&frame[1]) + 5 + 2 + 2;
    if(Verify_CRC8_Check_Sum(frame, 5) && Verify_CRC16_Check_Sum(frame, frame_length)) {
//		if(seq > frame[3]) {
//			return false;
//		}
//		else {
//			seq = frame[3];
//			return true;
//		}
		return true;
    }
    else {
        return false;
    }
}

//u8 blink_time=0;
//// ������˸��ʾ����
//void data_blink(void) {
//	blink_time++;
//	if(blink_time<60) {
////		blink_time=0;
//		extShowData.data3=8;
//		return;
//	}
//	if(blink_time<120) {
//		extShowData.data3 = 8.8888888888888888f;
//		return;
//	}
//	if(blink_time>120)  {
//		blink_time=0;
//		return;
//	}
//}



void update_from_dma(void) {
	int i;
	u8 USART6_dma_x2[2*USART6_dma_rx_len];
	memcpy(USART6_dma_x2,USART6_dma,USART6_dma_rx_len);
	memcpy(USART6_dma_x2+USART6_dma_rx_len,USART6_dma,USART6_dma_rx_len);

//	for(i=0;i<USART6_dma_rx_len;i++) {
//		frame_interpret(USART6_dma_x2+i);
//	}
	frame_interpret(USART6_dma_x2, 2*USART6_dma_rx_len);
	return;
}


bool vrerify_frame(uint8_t * frame) {
    /* frame length = header(5) + cmd_id(2) + data(n) + crc16(2) */
    uint16_t frame_length = 5 + 2 + _bytes2u16(&frame[1]) + 2;
    return ( Verify_CRC8_Check_Sum(frame, sizeof(ext_frame_header_t)) 
        && Verify_CRC16_Check_Sum(frame, frame_length) );
}
bool frame_interpret(uint8_t * _frame, uint16_t size) {
    for(uint16_t i=0; i<size; i++) {
        if(_frame[i]==REFEREE_FRAME_HEADER_SOF && vrerify_frame(&_frame[i])) {
//            static uint8_t seq_ex = 0;
//            uint8_t seq = frame[3];
//            if(frame[3]<=seq_ex && (seq_ex-frame[3])>=240) {
//                continue;
//            }
//            seq_ex = seq;
						uint8_t * frame = &_frame[i];
						uint16_t length = (uint16_t)(frame[1] | (frame[2]<<8));
						i += length + 7;
            
            ext_cmd_id_t cmd_id = (ext_cmd_id_t)(frame[5] | (frame[6]<<8));

//u8 blood_seq=0;
//// ʹ����������֡����ȫ��������Ϣ��ؽṹ�塣(��У��)
//u8 frame_interpret(uint8_t * frame) { 
//    if(Verify_frame(frame) == true) {
//		if(seq_real>=frame[3]&&(seq_real-frame[3])<100) {
//			return false;
//		}
////		offical_data_flag=0;
//		memcpy(&cmd_id,&frame[5],2);
//    //cmdID = _bytes2u16(&frame[5]);/////////////////////////////
//		seq_real=frame[3];
        switch(cmd_id) {
					case 0x0001:ext_game_state_interpret(&frame[7]);break;
					case 0x0002:ext_game_result_interpret(&frame[7]);break;
					case 0x0003:ext_game_robot_survivors_interpret(&frame[7]);break;
				  case 0x0101:ext_event_data_interpret(&frame[7]);break;
					case 0x0102:ext_supply_projectile_action_interpret(&frame[7]);break;
			  	case 0x0103:ext_supply_projectile_booking_interpret(&frame[7]);break;
					case 0x0201:ext_game_robot_state_interpret(&frame[7]);break;
			  	case 0x0202:ext_power_heat_data_interpret(&frame[7]);break;
					case 0x0203:ext_game_robot_pos_interpret(&frame[7]);break;
					case 0x0204:ext_buff_musk_interpret(&frame[7]);break;
					case 0x0205:aerial_robot_energy_interpret(&frame[7]);break;
					case 0x0206:ext_robot_hurt_interpret(&frame[7]);break;
					case 0x0207:ext_shoot_data_interpret(&frame[7]);break;
					case 0x0301:ext_student_interactive_header_data_interpret(&frame[7]);break;
					
           // case 1: extGameRobotState_interpret(&frame[7]);break;  // ����������Ϣ
            //case 2: extRobotHurt_interpret(&frame[7]);break; // �˺�����
            //case 3: extShootData_interpret(&frame[7]);break;  // ʵʱ�������
						//case 4: extPowerHeatData_interpret(&frame[7]);break;  //ʵʱ������������
					  //case 8: extGameRobotPos_interpret(&frame[7]);break;  //// ������λ�ú�ǹ�ڳ�����Ϣ
			//case 5:StudentPropInfo_interpret(&frame[7]);break;  //������Ϣ
            default: break;
        }
		// return true;
    }
//	else {
//		return false;
//	}
}
		return true;
}

// ����
// ʹ�ö��뵥�ֽ�������ȫ��������Ϣ��ؽṹ��, 
// �����ۻ��ֽ�Ϊһ�������ݰ�ʱ �ŵ���frame_interpret���� ��������ؽṹ�塣
//void referee_info_update(uint8_t single_byte) {
//    // ����ͷ֡����ʼ������referee_message[]
//    if(single_byte != sof) {
//        message_byte_counter++;  // ��1��ʼ��������
//        referee_message[message_byte_counter] = single_byte;
//    }
//    // ����ͷ֡
//    if(single_byte == sof) {
//        frame_interpret(referee_message);  // ʹ����������֡����ȫ��������Ϣ��ؽṹ�塣
//        referee_message[0] = sof;
//        message_byte_counter = 0; // ����
//    }
//}


// �Զ�������֡, ��װ��ָ��custom_frame������ = 5+2+13+2 = 22
                                              //13+6+5+2+2=28
// ����ǰ��ȷ��ȫ�ֱ���extShowData�ṹ���Ѹ���ֵ
//void custom_frame_pack(uint8_t * custom_frame) {
//    custom_frame[0] = sof;
//    custom_frame[1] = 12;  // data length
//    custom_frame[2] = 0x00;
//    custom_frame[3] = custom_info_counter;  // seq�����
//    Append_CRC8_Check_Sum(custom_frame, 5);
//    // cmdID
//    custom_frame[5] = 0x05;
//    custom_frame[6] = 0x00;
//    // �Զ������ݣ�ÿ��4�ֽ�(float)
//    float2bytes(extShowData.data1, &custom_frame[7]);
//    float2bytes(extShowData.data2, &custom_frame[11]);
//    float2bytes(extShowData.data3, &custom_frame[15]);
//		extShowData.mask = custom_frame[19];
//    Append_CRC16_Check_Sum(custom_frame, 22);
//    // ����������
//    custom_info_counter++;
//    return;
//}

void referee_send_client(ext_id_t target_id, float data[3], ext_client_custom_data_mask_t masks) {
    static ext_client_custom_data_t client_data;
    /* Header data */
    client_data.header.sof = REFEREE_FRAME_HEADER_SOF;
    client_data.header.seq ++;
    client_data.header.data_length = sizeof(client_data) 
        - sizeof(client_data.header) - sizeof(client_data.cmd_id) - sizeof(client_data.crc16);
    Append_CRC8_Check_Sum((uint8_t*)&client_data.header, sizeof(client_data.header));
    client_data.cmd_id = robot_interactive_data;
    /* Frame data */
    client_data.data_id = REFEREE_STUDENT_CLIENT_SOF;
    client_data.sender_id = MY_ROBOT_ID;
    client_data.client_id = target_id;
    memcpy((uint8_t*)client_data.data, (uint8_t*)data, sizeof(client_data.data));
    client_data.masks = masks;
    /* Calc CRC16 */
    Append_CRC16_Check_Sum((uint8_t*)&client_data, sizeof(client_data));
    /* Send out data */
		memcpy(Personal_Data,(u8*)&client_data,sizeof(client_data));
//		DMA_Cmd(DMA2_Stream6,ENABLE);
//		USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);
		RefereeSend(sizeof(client_data));
}


void referee_send_client_graphic(ext_id_t target_id,ext_client_graphic_draw_t *graphic_draw){
	
		static ext_robot_graphic_data_t robot_data;
		
		robot_data.header.sof= REFEREE_FRAME_HEADER_SOF;
		robot_data.header.seq++;
		robot_data.header.data_length=sizeof(robot_data)- sizeof(robot_data.header) - sizeof(robot_data.cmd_id) - sizeof(robot_data.crc16);
		Append_CRC8_Check_Sum((uint8_t*)&robot_data.header, sizeof(robot_data.header));
		
		robot_data.cmd_id = robot_interactive_data;
    robot_data.data_id = 0x0100;
    robot_data.sender_id = MY_ROBOT_ID;
    robot_data.robot_id = target_id;
		// memcpy(&robot_data.graphic_data ,(uint8_t*)&graphic_draw, sizeof(robot_data.graphic_data));
		robot_data.graphic_data = *graphic_draw;
		Append_CRC16_Check_Sum((uint8_t*)&robot_data, sizeof(robot_data));
		//spDMA.mem2mem.copy((uint32_t)robot_data.data, (uint32_t)graphic_draw, size);
    /* Send out data */
//		return spDMA.controller.start(spDMA_USART6_tx_stream, 
//        (uint32_t)&robot_data, (uint32_t)USART6->DR, sizeof(robot_data));
		memcpy(Personal_Data,(u8*)&robot_data,sizeof(robot_data));
		RefereeSend(sizeof(robot_data));
}

void referee_clear_client_graphic(){
		ext_client_graphic_draw_t graphic_draw;
		graphic_draw.operate_tpye = 6;
		referee_send_client_graphic(MY_CLIENT_ID,&graphic_draw);
}

bool first_draw=true;

void send_graphic(void)
{
	ext_client_graphic_draw_t graphic_draw;
	int x = 500;
	if(first_draw){
		char *name = "danmu";
		char *value = "awsl";
		graphic_draw.operate_tpye = 1;
		graphic_draw.graphic_tpye = 6;
		graphic_draw.layer = 0;
		graphic_draw.color = 1;
		graphic_draw.width = 3;
		graphic_draw.start_x = x;
		graphic_draw.start_y =300;
		graphic_draw.radius = 30;
		graphic_draw.text_lenght = strlen(value);
		memcpy(graphic_draw.graphic_name, (uint8_t*)name, strlen(name));
		memcpy(graphic_draw.text, (uint8_t*)value, strlen(value));
		
//		graphic_draw.operate_tpye = 1;
//		graphic_draw.graphic_tpye = 3;
//		graphic_draw.layer = 1;
//		graphic_draw.color = 1;
//		graphic_draw.width = 5;
//		graphic_draw.start_x = 500;
//		graphic_draw.start_y = 500;
//		graphic_draw.radius = 100;	
//		first_draw = false;
	}
	else{
		char *name = "dmaku";
		char *value = "awsl";
		x +=50;
		if(x>1000) x = 500;
		graphic_draw.operate_tpye = 2;
		graphic_draw.graphic_tpye = 6;
		graphic_draw.layer = 0;
		graphic_draw.color = 1;
		graphic_draw.width = 3;
		graphic_draw.start_x = x;
		graphic_draw.start_y =540;
		graphic_draw.radius = 100;
		graphic_draw.text_lenght = strlen(value);
		memcpy(graphic_draw.graphic_name, (uint8_t*)name, strlen(name));
		memcpy(graphic_draw.text, (uint8_t*)value, strlen(value));
//		graphic_draw.operate_tpye = 0;
	}
	referee_send_client_graphic(MY_CLIENT_ID,&graphic_draw);
}


void Send_Middle_rectangle(int level, int color,int x_length,int y_length)
{
	ext_client_graphic_draw_t graphic_draw2;
	char name_temp=(char)(level+30);
	char *name = &name_temp;
	graphic_draw2.operate_tpye = 1;
	graphic_draw2.graphic_tpye = 2;
	graphic_draw2.layer = level;
	graphic_draw2.color = color;
	graphic_draw2.width = 2;
	graphic_draw2.start_x = (int)(960-x_length/2.0f);
	graphic_draw2.start_y = (int)(540-y_length/2.0f);
	graphic_draw2.end_x=(int)(960+x_length/2.0f);
	graphic_draw2.end_y=(int)(540+y_length/2.0f);
	memcpy(graphic_draw2.graphic_name, (uint8_t*)name, strlen(name));
	referee_send_client_graphic(MY_CLIENT_ID,&graphic_draw2);
}

void Send_SOS(void)
{
	ext_client_graphic_draw_t graphic_draw;
	char *name = "awsl";
	char *value = "SOS";
	graphic_draw.operate_tpye = 1;
	graphic_draw.graphic_tpye = 6;
	graphic_draw.layer = 9;
	graphic_draw.color = 5;
	graphic_draw.width = 20;
	graphic_draw.start_x =1500 ;
	graphic_draw.start_y =500;
	graphic_draw.radius = 70;
	graphic_draw.text_lenght = strlen(value);
	memcpy(graphic_draw.graphic_name, (uint8_t*)name, strlen(name));
	memcpy(graphic_draw.text, (uint8_t*)value, strlen(value));
	referee_send_client_graphic(MY_CLIENT_ID,&graphic_draw);
}

void Clear_SOS(void)
{
	ext_client_graphic_draw_t graphic_draw;
	char *name = "awsl";
	graphic_draw.operate_tpye = 3;
	graphic_draw.layer = 9;
	memcpy(graphic_draw.graphic_name, (uint8_t*)name, strlen(name));
	referee_send_client_graphic(MY_CLIENT_ID,&graphic_draw);
}






float data_temp[3]={0,0,0};
ext_client_custom_data_mask_t mask_temp;
long long int send_data_count=0;
void send_data_to_client(void)
{
	send_data_count++;
	if(raging_mode==1)
	{
		if(send_data_count%2==0)
		{
			if(mask_temp.masks_bits.led1==1)
			{
				mask_temp.masks_bits.led1=0;
				mask_temp.masks_bits.led2=0;
				mask_temp.masks_bits.led3=0;
				mask_temp.masks_bits.led4=0;
				mask_temp.masks_bits.led5=0;
				mask_temp.masks_bits.led6=0;
			}
			else
			{
				mask_temp.masks_bits.led1=1;
				mask_temp.masks_bits.led2=1;
				mask_temp.masks_bits.led3=1;
				mask_temp.masks_bits.led4=1;
				mask_temp.masks_bits.led5=1;
				mask_temp.masks_bits.led6=1;
			}
		}
	}
	else
	{
		if(climb_mode_flag==1)
		{
			mask_temp.masks_bits.led1=0;
		}
		else
		{
			mask_temp.masks_bits.led1=1;
		}
		#ifdef SUPER_CAPACITOR
			if(Lost_Connection_Count<100)
			{
				mask_temp.masks_bits.led2=1;
			}
			else
			{
				mask_temp.masks_bits.led2=0;
			}
		#endif
//		if(bullet==1)
//		{
//			mask_temp.masks_bits.led3=1;
//		}
//		else
//		{
//			mask_temp.masks_bits.led3=0;
//		}
//		if(friction_state_flag==1)
//		{
//			mask_temp.masks_bits.led6=1;
//		}
//		else
//		{
//			mask_temp.masks_bits.led6=0;
//		}
//		if(shoot_up_speed_flag==1)
//		{
//			mask_temp.masks_bits.led5=0;
//		}
//		else
//		{
//			mask_temp.masks_bits.led5=1;
//		}
//		if(adi_die_flag==0)
//		{
//			mask_temp.masks_bits.led4=1;
//		}
//		else
//		{
//			mask_temp.masks_bits.led4=0;
//		}
	}
	
	
	//data_temp[0]=remain_bullet;
	
	if(!if_newframe||timeout_count>1000)
	{
		data_temp[1]=99.99;
	}
	else
	{	
		data_temp[1]=(int)fram.yaw+((int)fram.pitch)/100.0f;
	}
#ifdef INFANTRY_Jiangsu
	if(hand_cleaning_sent)
	{
		data_temp[2]=(float)hand_zero_clear;
	}
	else
	{
		data_temp[2]=0;
	}
#endif
	
	
	
	//data_temp[2]=(15400-cap_receive.CapVol)/36.0f;//100Ϊ����ֵ��30Ϊ�͵��������������
	referee_send_client(MY_CLIENT_ID,data_temp,mask_temp);
}

