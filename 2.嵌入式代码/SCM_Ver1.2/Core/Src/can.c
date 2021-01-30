/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "SCM.h"
#include "ina226.h"
CAN_TxHeaderTypeDef  TxHeader;
CAN_RxHeaderTypeDef  RxHeader;
uint8_t CanTxData[8],CanRxData[8];
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 3;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_6TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */
    CAN_FilterTypeDef  CanFilterConfig;

    CanFilterConfig.FilterBank = 0;
    CanFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    CanFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    CanFilterConfig.FilterIdHigh = 0x0000;
    CanFilterConfig.FilterIdLow = 0x0000;
    CanFilterConfig.FilterMaskIdHigh = 0x0000;
    CanFilterConfig.FilterMaskIdLow = 0x0000;
    CanFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    CanFilterConfig.FilterActivation = ENABLE;
    CanFilterConfig.SlaveStartFilterBank = 0;  //For single CAN instances, this parameter is meaningless.

    if(HAL_CAN_ConfigFilter(canHandle, &CanFilterConfig) != HAL_OK){
    	 Error_Handler();
    }
    if(HAL_CAN_ActivateNotification(canHandle,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK){
		 Error_Handler();
	}
    if(HAL_CAN_Start(canHandle) != HAL_OK){
		 Error_Handler();
	}
  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CanRxData) != HAL_OK){
		Error_Handler();
	}
	if(RxHeader.StdId==0x300 && SCM_CanReceiveFlag==SCM_CAN_Ready){
		SCM.CAN_ConnectWDT=20;

		if((CanRxData[0] & 0x01) == 0x01)
			SCM_Next.Buck=SCM_Buck_Run;
		else
			SCM_Next.Buck=SCM_Buck_Off;

		if((CanRxData[0] & 0x02) == 0x02)
			SCM_Next.Mode=SCM_Mode_Cap;
		else
			SCM_Next.Mode=SCM_Mode_Bat;

		SCM_Next.SetPow=(CanRxData[2]<<8|CanRxData[3])*25;

		SCM_CanReceiveFlag=SCM_CAN_Busy;
	}

}
void SCM_CAN_Transmit(void){
	uint32_t retMail;

	TxHeader.StdId=0x301;
//	TxHeader.ExtId=0x00;
	TxHeader.IDE=CAN_ID_STD;
	TxHeader.RTR=CAN_RTR_DATA;
	TxHeader.DLC=8;
	TxHeader.TransmitGlobalTime=DISABLE;
	CanTxData[0]=SCM.Mode<<1 | SCM.Buck;
	CanTxData[1]=0;
	CanTxData[2]=InaPow[1]>>8;
	CanTxData[3]=InaPow[1];
	CanTxData[4]=InaPow[0]>>8;
	CanTxData[5]=InaPow[0];
	CanTxData[6]=InaVol[1]>>8;
	CanTxData[7]=InaVol[1];
//	HAL_CAN_ActivateNotification(&hcan,CAN_IT_TX_MAILBOX_EMPTY);
	if(HAL_CAN_AddTxMessage(&hcan,&TxHeader,CanTxData, &retMail)!=HAL_OK){
		Error_Handler();
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
