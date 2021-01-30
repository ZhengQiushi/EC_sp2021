/*
* @Author: Yaadon
* @Date:   2020-10-21 13:17:43
* @Last Modified by:   Yaadon
* @Last Modified time: 2020-10-31 20:47:03
*/
#include "main.h"
#define uart8datasize 6
u8 u8_sendbuffer[uart8datasize];
void UART8_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);		
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0, GPIO_AF_UART8);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1, GPIO_AF_UART8);
	
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart8;
	gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &gpio);

	USART_DeInit(UART8);
	uart8.USART_BaudRate = 115200;
	uart8.USART_WordLength = USART_WordLength_8b;
	uart8.USART_StopBits = USART_StopBits_1;
	uart8.USART_Parity = USART_Parity_No;
	uart8.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	uart8.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART8,&uart8);
	
//	USART_ITConfig(USART2,USART_IT_TC,DISABLE);  
//	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE); 
//	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
	
	USART_Cmd(UART8,ENABLE);
	USART_ClearFlag(UART8, USART_FLAG_TC);
	
	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);

//	{
//	NVIC_InitTypeDef nvic;
//	nvic.NVIC_IRQChannel = USART2_IRQn;
//	nvic.NVIC_IRQChannelPreemptionPriority = 4;
//	nvic.NVIC_IRQChannelSubPriority = 0;
//	nvic.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvic);
//	}
	DMA_InitTypeDef dma;
	DMA_DeInit(DMA1_Stream0);
	
	dma.DMA_Channel = DMA_Channel_5;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(UART8->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)u8_sendbuffer;   
	dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma.DMA_BufferSize = uart8datasize;       
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Normal ;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream0,&dma);
	{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = DMA1_Stream0_IRQn;      
	nvic.NVIC_IRQChannelPreemptionPriority = 3;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);  
	}
	
	DMA_Cmd(DMA1_Stream0,DISABLE);							
	//DMA_Cmd(DMA1_Stream1,ENABLE);


}

void DMA1_Stream0_IRQHandler(void)
{

	 if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)!=RESET)//??DMA2_Steam7????  
    {   
			//USART_DMACmd(UART8,USART_DMAReq_Tx,DISABLE);
			DMA_Cmd(DMA1_Stream0,DISABLE);	
      DMA_ClearITPendingBit(DMA1_Stream0,DMA_IT_TCIF0);//??DMA2_Steam7??????  
			//DMA_Cmd(DMA1_Stream0,ENABLE);	
    }  
}

//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* ·¢ËÍÒ»¸ö×Ö½ÚÊý¾Ýµ½USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* µÈ´ý·¢ËÍÊý¾Ý¼Ä´æÆ÷Îª¿Õ */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

//void UART2_Send(u8 *buffer)
//{
//	u8 i=0;
//	while (i < 2) 
//	{
//		while((USART6->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
//		USART6->DR = (u8) buffer[i++];      
//	} 
//}

void UART_DMA_SEND(int data){
	int temp;
	//u8 buffer[16];
	int i=0;
	int len;
	for(i=0;i<uart8datasize;i++){
		u8_sendbuffer[i]=0x00;
	}
	i=0;
	if(data>=0){
		while(data/10!=0){
		temp=data-data/10*10;
		u8_sendbuffer[i]=temp+'0';
		i++;
		data/=10;
		}
		u8_sendbuffer[i]=data+'0';
		i++;
		len=i;
	}
	else{
		int neg_data=-data;
		while(neg_data/10!=0){
			temp=neg_data-neg_data/10*10;
			u8_sendbuffer[i]=temp+'0';
			i++;
			neg_data/=10;
		}
		u8_sendbuffer[i]=neg_data+'0';
		i++;
		u8_sendbuffer[i]='-';
		len=i+1;
	}
	u8 temp_char;
	for(i=0;i<len/2;i++){
			temp_char=u8_sendbuffer[i];
			u8_sendbuffer[i]=u8_sendbuffer[len-i-1];
			u8_sendbuffer[len-1-i]=temp_char;
		}
	u8_sendbuffer[len]='\r';
	//USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);
	DMA_Cmd(DMA1_Stream0,ENABLE);	
}
