/*
 * Connection.cpp
 *
 *  Created on: May 15, 2021
 *      Author: Yven Vogt
 */
#include "Connection.h"

Connect::Connect()
{
	SystemState = State_NotInit;
	TransmitQueueHandle = osMessageQueueNew(RECEIVE_BUFFER_SIZE , sizeof(uTransmitReceive_t), 0);
	ReceiveQueueHandle =  osMessageQueueNew(TRANSMIT_BUFFER_SIZE, sizeof(uTransmitReceive_t), 0);
	u8OwnClientName = 0U;
	u8ConnectedWith = 0U;
	bReceiveQueueFull = false;
	bTransmitQueueFull = false;
}

Connect::~Connect()
{

}

void Connect::vInit(uint32_t CLIENT)
{
	u8OwnClientName = CLIENT;
	u8ConnectedWith = 0;
	SystemState = State_Ready;
	TxHeader.StdId = CLIENT;
	TxHeader.ExtId = 0x00;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = sizeof(uTransmitReceive_t);
	TxHeader.TransmitGlobalTime = DISABLE;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode =CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if( HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

void Connect::vTransmit(uTransmitReceive_t& Transmit)
{
	if (osMessageQueueGetCount(TransmitQueueHandle) < TRANSMIT_BUFFER_SIZE)
	{
		osMessageQueuePut(TransmitQueueHandle,&Transmit, 0, 0);
		bTransmitQueueFull = false;
	}
	else
	{
		bTransmitQueueFull = true;
	}
}

void Connect::vTransmitState()
{
	uTransmitReceive_t Transmit;
	Transmit.sData.u8Receiver = (uint8_t)CLIENT_CONTROLLER;
	Transmit.sData.u8Mode = (uint8_t)Mode_StatusMassage;
	Transmit.sData.u8State = (uint8_t)SystemState;
	Transmit.sData.u8Sender = u8OwnClientName;

	vTransmit(Transmit);
}

void Connect::vSetConnectedWith(uint8_t u8NewConnectedWith)
{
	this->u8ConnectedWith = u8NewConnectedWith;
}

void Connect::vSendMessage()
{
	if (osMessageQueueGetCount(TransmitQueueHandle) > 0U)
	{
		for (uint16_t u16Counter = 0; u16Counter < osMessageQueueGetCount(TransmitQueueHandle); u16Counter++)
		{
			uTransmitReceive_t Transmit;
			osMessageQueueGet(TransmitQueueHandle, &Transmit, 0, 0);
			HAL_CAN_AddTxMessage(&hcan1, &TxHeader, Transmit.u8Data, &TxMailbox);
			while (HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox));
		}
	}
}

void Connect::vReceive()
{
	uTransmitReceive_t Receive;
	CAN_RxHeaderTypeDef RxHeader;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, Receive.u8Data);
	if ((Receive.sData.u8Receiver & u8OwnClientName) == u8OwnClientName)
	{
		if (osMessageQueueGetCount(ReceiveQueueHandle) < RECEIVE_BUFFER_SIZE)
		{
			osMessageQueuePut(ReceiveQueueHandle,&Receive, 0, 0);
			bReceiveQueueFull = false;
		}
		else
		{
			bReceiveQueueFull = true;
		}
	}
}

uint8_t Connect::GetNewMessage(uTransmitReceive_t& Receive)
{
	uint8_t u8Return = BUFFER_EMPTY;
	if (osMessageQueueGetCount(ReceiveQueueHandle) > 0U)
	{
		u8Return = BUFFER_OK;
		osMessageQueueGet(ReceiveQueueHandle, &Receive, 0, 0);
	}
	return u8Return;
}

