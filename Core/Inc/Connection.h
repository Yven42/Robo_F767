/*
 * Connection.h
 *
 *  Created on: May 15, 2021
 *      Author: Yven Vogt
 */
#ifndef INC_CONNECTION_H_
#define INC_CONNECTION_H_

#include "main.h"
#include "cmsis_os.h"

#define CLIENT_CONTROLLER 		0x80U
#define CLIENT_BODY_CONTROLLER	0x40U
#define CLIENT_HEAD_CONTROLLER	0x08U

#define TRANSMIT_BUFFER_SIZE	16U
#define RECEIVE_BUFFER_SIZE		16U

#define BUFFER_FULL 1U
#define BUFFER_EMPTY 255U
#define BUFFER_OK 0U

enum Mode
{
	Mode_StatusMassage = 1,
	Mode_NewServoValue = 2,
};

enum SystemState
{
	State_NotInit = 0,
	State_Ready   = 1,
	State_Error   = 2,
}typedef SystemState_t;

typedef StaticQueue_t osStaticMessageQDef_t;
extern CAN_HandleTypeDef hcan1;

struct Data
{
	uint8_t u8Receiver;
	uint8_t u8Mode;
	uint8_t u8State;
	uint8_t u8Sender;
	float fData;
}typedef uData_t;

union TransmitReceive
{
	uint8_t u8Data[sizeof(uData_t)];
	uData_t sData;
}typedef uTransmitReceive_t;


class Connect
{
public:

	Connect();
	~Connect();
	void vInit(uint32_t CLIENT);
	void vTransmit(uTransmitReceive_t& Transmit);
	void vReceive();
	void vTransmitState();
	void vSendMessage();
	uint8_t GetNewMessage(uTransmitReceive_t& Receive);
	void vSetConnectedWith(uint8_t u8NewConnectedWith);

private:

	bool bReceiveQueueFull;
	bool bTransmitQueueFull;

	uint32_t TxMailbox;
	SystemState_t SystemState;

	osMessageQueueId_t TransmitQueueHandle;
	osMessageQueueId_t ReceiveQueueHandle;

	uint8_t u8OwnClientName;
	uint8_t u8ConnectedWith;

	CAN_FilterTypeDef sFilterConfig;
	CAN_TxHeaderTypeDef TxHeader;
};



#endif /* INC_CONNECTION_H_ */
