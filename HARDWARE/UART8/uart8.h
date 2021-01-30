#ifndef __UART8_H__
#define __UART8_H__

#include "stm32f4xx.h"


//void UART7_Send(u8 *buffer);
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);

void UART8_Init(void);
void UART_DMA_SEND(int data);

#endif
