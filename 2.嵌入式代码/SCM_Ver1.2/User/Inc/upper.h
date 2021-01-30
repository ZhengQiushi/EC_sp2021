 /*
  ******************************************************************************
  * @author         : Crazt
  * @date			: 2020.5.31
  * @brief          : Super capacitor module control program
  * 				  For Tongji SuperPower 2020
  ******************************************************************************

  ******************************************************************************
 */
#include "stm32f1xx.h"

typedef union
{
	float value;
	uint32_t value_i;
	unsigned char data[4];
}SerialDataType;

#define  UART_RxBUFFER_SIZE  800
#define  UART_TxBUFFER_SIZE  800

#define  UART_SerialData_SIZE  400

extern uint16_t UartTransmitEnableTime;

//当前的数据帧种类以及帧总长定义
#define	 SP_DATA_NUM 4

#define  SP00_LenRx 3
#define  SP01_LenRx 9
#define  SP02_LenRx 4
#define  SP03_LenRx 6

#define  SP00_LenTx 3
#define  SP01_LenTx 9
#define  SP02_LenTx 10
#define  SP03_LenTx 6

void UART_InitForUpper(UART_HandleTypeDef *huart);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);
void USER_UART1_DMA_Transmit2Upper();
uint8_t crc_check(SerialDataType* Data,uint16_t Len);
