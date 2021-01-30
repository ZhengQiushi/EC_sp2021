#include "stdio.h"
#include "string.h"

#include "crc32.h"
#include "upper.h"
#include "usart.h"
#include "SCM.h"
#include "mpu9255.h"
uint8_t uart_receive_buff[UART_RxBUFFER_SIZE];
uint8_t uart_transmit_buff[UART_TxBUFFER_SIZE];

SerialDataType RxData[UART_SerialData_SIZE];
SerialDataType TxData[UART_SerialData_SIZE];
//TODO:接收与发送无法同时进行的临时解决方案，后续修改。
uint16_t UartTransmitEnableTime=0;

const uint16_t SerialFrameLenRx[SP_DATA_NUM] = {
    SP00_LenRx,
    SP01_LenRx,
    SP02_LenRx,
    SP03_LenRx
};
const uint16_t SerialFrameLenTx[SP_DATA_NUM] = {
    SP00_LenTx,
    SP01_LenTx,
    SP02_LenTx,
    SP03_LenTx
};

//UART DMA Receive Reference：https://blog.csdn.net/qq_17351161/article/details/90415407
//该部分代码应当分别置于xxxit.c 以及 usart.c中更为合适 ，但是为了方便还是放在这里了
void UART_InitForUpper(UART_HandleTypeDef *huart)
{
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	HAL_UART_Receive_DMA(huart, (uint8_t*)uart_receive_buff, UART_RxBUFFER_SIZE);     //设置DMA传输，将串口1的数据存到recvive_buff中，

}
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //判断是否是串口1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //清除空闲中断标志（否则会一直不断进入中断）
            //printf("\r\nUART1 Idle IQR Detected\n");
            USER_UART_IDLECallback(huart);                          //调用中断处理函数
        }
    }
}

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	UartTransmitEnableTime=180;
    HAL_UART_DMAStop(&huart1);                                                    		 //停止本次DMA传输
    uint16_t data_length = (uint16_t)UART_RxBUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   //计算接收到的数据长度
//    //测试函数：发送接收到的数据
//    printf("Receive Data(length = %d): \n",data_length);
//    HAL_UART_Transmit(&huart1,uart_receive_buff,data_length,0x200);
//    printf("\r\n");

    //Process Data Frame from upper PC
    for(int i=0;i<data_length;i++)
    	RxData[i/4].data[i%4]=uart_receive_buff[i];

    uint16_t index = 0;
    while (index  < data_length/4)
    {
	   if (RxData[index].data[0] == 'S' &&
		   RxData[index].data[1] == 'P' &&
		   RxData[index].data[2] == '0'
		   )
	   {
		   //SP00
		   if (RxData[index].data[3] == '0')
		   {
			   int crcIndex=index;
//			   int index_temp = index + 1;
			   index += SerialFrameLenRx[0];
			   if(crc_check(&RxData[crcIndex],SerialFrameLenRx[0])==0)
				   continue;
			   continue;
		   }
		   //SP01
		   if (RxData[index].data[3] == '1')
		   {
			   int crcIndex=index;
//			   int index_temp = index + 1;
			   index += SerialFrameLenRx[1];

			   if(crc_check(&RxData[crcIndex],SerialFrameLenRx[1])==0)
				   continue;
			   continue;
		   }
		   //SP02
		   if (RxData[index].data[3] == '2')
		   {
			   int crcIndex=index;
			   int index_temp = index + 1;
			   index += SerialFrameLenRx[2];
			   if(crc_check(&RxData[crcIndex],SerialFrameLenRx[2])==0)
				   continue;

			   SCM_Next.Buck 	     =  RxData[index_temp].data[0] & 1;
			   SCM_Next.Mode 		 = (RxData[index_temp].data[0] & (1<<1))>>1;
			   SCM_Next.ForceUseCap  = (RxData[index_temp].data[0] & (1<<2))>>2;
			   index_temp++;
			   SCM_Next.SetPow  = RxData[index_temp++].value*1000;

			   continue;
		   }
		   //SP03
		   if (RxData[index].data[3] == '3')
		   {
			   int crcIndex=index;
			   int index_temp = index + 1;
			   index += SerialFrameLenRx[3];

			   if(crc_check(&RxData[crcIndex],SerialFrameLenRx[3])==0)
				   continue;

			   SCM_SettingsNext.Cap_MaxVol_H = RxData[index_temp++].value*1000;
			   SCM_SettingsNext.Cap_MaxVol_L = RxData[index_temp++].value*1000;
			   SCM_SettingsNext.Cap_MinVol_H = RxData[index_temp++].value*1000;
			   SCM_SettingsNext.Cap_MinVol_L = RxData[index_temp++].value*1000;


			   continue;
		   }
	   }
	   else
		   index++;
    }

//    memset(RxData,0,UART_SerialData_SIZE);                                     //清零接收缓冲区
    data_length = 0;
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart_receive_buff, UART_RxBUFFER_SIZE);     //重启开始DMA传输 每次255字节数据
    //TODO:如果单次传输大小超限怎么办,会从头开始覆盖吗？
}

//UART DMA Transmit
void USER_UART1_DMA_Transmit2Upper()
{
	uint16_t index=0,data_size=0;
	//SP02 SCM_Status Data Frame
	TxData[index].data[0]='S';
	TxData[index].data[1]='P';
	TxData[index].data[2]='0';
	TxData[index].data[3]='2';
	index++;
	TxData[index].data[0]=0x00;
	TxData[index].data[0]|=SCM.Buck;
	TxData[index].data[0]|=(SCM.Mode<<1);
	TxData[index].data[0]|=(SCM.ForceUseCap<<2);
	index++;

	TxData[index++].value=(float)SCM.SetPow/1000.0;
	TxData[index++].value=(float)SCM.CapPow/1000.0;
	TxData[index++].value=(float)SCM.CapCur/1000.0;
	TxData[index++].value=(float)SCM.CapVol/1000.0;
	TxData[index++].value=(float)SCM.BatPow/1000.0;
	TxData[index++].value=(float)SCM.BatCur/1000.0;
	TxData[index++].value=(float)SCM.BatVol/1000.0;
	//CRC Value
	TxData[index].value_i=crc32(&TxData[0].data[0],(SerialFrameLenTx[2]-1)*4);
	index++;
	data_size+=index;

	//SP01 IMU
	if(MPU9255_real_data_t.accel[0]>0.001f||MPU9255_real_data_t.accel[1]>0.001f)
	{
		index=0;
		TxData[data_size+index].data[0]='S';
		TxData[data_size+index].data[1]='P';
		TxData[data_size+index].data[2]='0';
		TxData[data_size+index].data[3]='1';
		index++;
		TxData[data_size+index++].value=MPU9255_real_data_t.accel[0];
		TxData[data_size+index++].value=MPU9255_real_data_t.accel[1];
		TxData[data_size+index++].value=MPU9255_real_data_t.accel[2];
		TxData[data_size+index++].value=MPU9255_real_data_t.speed[0];
		TxData[data_size+index++].value=MPU9255_real_data_t.speed[1];
		TxData[data_size+index++].value=MPU9255_real_data_t.speed[2];
		TxData[data_size+index++].value=MPU9255_real_data_t.temp;

		TxData[data_size+index].value_i=crc32(&TxData[data_size].data[0],(SerialFrameLenTx[1]-1)*4);
		index++;
		data_size+=index;
	}


	index=0;
	//SP03 SCM Advanced Settings Frame
	TxData[data_size+index].data[0]='S';
	TxData[data_size+index].data[1]='P';
	TxData[data_size+index].data[2]='0';
	TxData[data_size+index].data[3]='3';
	index++;
	TxData[data_size+index++].value=(float)SCM_Settings.Cap_MaxVol_H/1000.0;
	TxData[data_size+index++].value=(float)SCM_Settings.Cap_MaxVol_L/1000.0;
	TxData[data_size+index++].value=(float)SCM_Settings.Cap_MinVol_H/1000.0;
	TxData[data_size+index++].value=(float)SCM_Settings.Cap_MinVol_L/1000.0;

//	uint32_t temp3=crc32(&TxData[0].data[0],SerialFrameLenTx[3]);
	TxData[data_size+index].value_i=crc32(&TxData[data_size].data[0],(SerialFrameLenTx[3]-1)*4);
	index++;
	data_size+=index;

	for(int i=0;i< data_size*4 ;i++)
		uart_transmit_buff[i]=TxData[i/4].data[i%4];
    HAL_UART_Transmit_DMA(&huart1,(uint8_t*)uart_transmit_buff,data_size*4);
//	 HAL_UART_Transmit_DMA(&huart1,&TxData[0].data[0],data_size*4);
}

uint8_t crc_check(SerialDataType* Data,uint16_t Len)
{
	const uint32_t ExpectValue= (Data+Len-1)->value_i;
	uint32_t CalValue=crc32(&(Data->data[0]), (Len-1)*4);
	if(CalValue == ExpectValue)
		return 1;
	else
		return 0;
}












