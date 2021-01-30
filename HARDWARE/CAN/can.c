#include "main.h"	


volatile int16_t current_position_201; 
volatile int16_t current_speed_201;
volatile int16_t previous_position_201;
volatile float continuous_current_position_201;
volatile int16_t current_position_202; 
volatile int16_t current_speed_202;
volatile int16_t previous_position_202;
volatile float continuous_current_position_202;
volatile int16_t current_position_203; 
volatile int16_t current_speed_203;
volatile int16_t previous_position_203;
volatile float continuous_current_position_203;
volatile int16_t current_position_204; 
volatile int16_t current_speed_204;
volatile int16_t previous_position_204;
volatile float continuous_current_position_204;
u8 rotate_201_count=0;
u8 rotate_202_count=0;
u8 rotate_203_count=0;
u8 rotate_204_count=0;

//CAN1_Mode_Init
//功能：CAN1初始化函数
//参数：传入tsjw,tbs2,tbs1,brp,mode 决定CAN1的模式和波特率
//返回值：0
uint8_t CAN1_Mode_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = mode;
    CAN_InitStructure.CAN_SJW = tsjw;
    CAN_InitStructure.CAN_BS1 = tbs1;
    CAN_InitStructure.CAN_BS2 = tbs2;
    CAN_InitStructure.CAN_Prescaler = brp;
    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    return 0;
}



void CAN1_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;
	
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
		{
				
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
					if(rx_message.StdId == 0x201)      //201         
					{ 		
							current_position_201 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
						  current_speed_201=(rx_message.Data[2]<<8)|rx_message.Data[3];
					}	
					else if(rx_message.StdId == 0x202)      //202       
					{ 		
							current_position_202 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
						  current_speed_202=(rx_message.Data[2]<<8)|rx_message.Data[3];
					}	
					else if(rx_message.StdId == 0x203)      //203         
					{ 		
							current_position_203 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
						  current_speed_203=(rx_message.Data[2]<<8)|rx_message.Data[3];
					}	
					else if(rx_message.StdId == 0x204)      //204         
					{ 		
							current_position_204 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
						  current_speed_204=(rx_message.Data[2]<<8)|rx_message.Data[3];
					}	
				CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}





void continue_value(void)   //增大值 使之连续
{
		if(!flag_Ready)
	{
		continuous_current_position_201=current_position_201;
	}
	else
	{
		if(abs(previous_position_201-current_position_201)>6000)
		{
			if(previous_position_201<1000)
			{
				rotate_201_count-=1;
			}
			else
				rotate_201_count+=1;
		}
		continuous_current_position_201=current_position_201+rotate_201_count*8192;
	}
	previous_position_201=current_position_201;
	
	
	if(!flag_Ready)
	{
		continuous_current_position_202=current_position_202;
	}
	else
	{
		if(abs(previous_position_202-current_position_202)>6000)
		{
			if(previous_position_202<1000)
			{
				rotate_202_count-=1;
			}
			else
				rotate_202_count+=1;
		}
		continuous_current_position_202=current_position_202+rotate_202_count*8192;
	}
	previous_position_202=current_position_202;
	
	if(!flag_Ready)
	{
		continuous_current_position_203=current_position_203;
	}
	else
	{
		if(abs(previous_position_203-current_position_203)>6000)
		{
			if(previous_position_203<1000)
			{
				rotate_203_count-=1;
			}
			else
				rotate_203_count+=1;
		}
		continuous_current_position_203=current_position_203+rotate_203_count*8192;
	}
	previous_position_203=current_position_203;
	
	if(!flag_Ready)
	{
		continuous_current_position_204=current_position_204;
	}
	else
	{
		if(abs(previous_position_204-current_position_204)>6000)
		{
			if(previous_position_204<1000)
			{
				rotate_204_count-=1;
			}
			else
				rotate_204_count+=1;
		}
		continuous_current_position_204=current_position_204+rotate_204_count*8192;
	}
	previous_position_204=current_position_204;
}

void CAN1_Cmd(int16_t l_data1,int16_t l_data2,int16_t R_data1,int16_t R_data2)
{
	 CanTxMsg tx_message;
    
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
	
    tx_message.Data[0] = (uint8_t)(l_data1 >> 8);
    tx_message.Data[1] = (uint8_t)l_data1;
    tx_message.Data[2] = (uint8_t)(l_data2 >> 8);
    tx_message.Data[3] = (uint8_t)l_data2;
		tx_message.Data[4] = (uint8_t)(R_data1 >> 8);
    tx_message.Data[5] = (uint8_t)R_data1;
    tx_message.Data[6] = (uint8_t)(R_data2 >> 8);
    tx_message.Data[7] = (uint8_t)R_data2;
    
    CAN_Transmit(CAN1,&tx_message);
}



