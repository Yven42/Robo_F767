/*
 * Device.cpp
 *
 *  Created on: Apr 15, 2021
 *      Author: Yven Vogt
 */

#include "main.h"
#include "cmsis_os.h"
#include "Device.h"


Device::Device()
{
	//StartValues
	eSelected_Limps = Arm_Right;
	fServoValue[LIMBS_ARM_RIGHT][ARM_SHOULDER_FORWARD] = 60;
	fServoValue[LIMBS_ARM_RIGHT][ARM_SHOULDER_UP] 		= 60;
	fServoValue[LIMBS_ARM_RIGHT][ARM_ELBOW] 			= 60;
	fServoValue[LIMBS_ARM_RIGHT][ARM_HAND_ROTATE] 		= 60;
	fServoValue[LIMBS_ARM_RIGHT][ARM_HAND_OPEN] 		= 60;

	fServoValue[LIMBS_ARM_LEFT][ARM_SHOULDER_FORWARD] 	= 40;
	fServoValue[LIMBS_ARM_LEFT][ARM_SHOULDER_UP] 		= 40;
	fServoValue[LIMBS_ARM_LEFT][ARM_ELBOW] 			= 40;
	fServoValue[LIMBS_ARM_LEFT][ARM_HAND_ROTATE] 		= 40;
	fServoValue[LIMBS_ARM_LEFT][ARM_HAND_OPEN] 		= 40;

	fServoValue[LIMBS_LEG_RIGHT][LEG_UP_DOWN] 				= 160;
	fServoValue[LIMBS_LEG_RIGHT][LEG_FORWARD_BACKWARD] 	= 160;
	fServoValue[LIMBS_LEG_RIGHT][LEG_KNEE] 				= 160;
	fServoValue[LIMBS_LEG_RIGHT][LEG_FOOT_FORWARD_BACKWARD]= 160;
	fServoValue[LIMBS_LEG_RIGHT][LEG_FOOT_RIGHT_LEFT] 		= 160;

	fServoValue[LIMBS_LEG_LEFT][LEG_UP_DOWN] 				= 10;
	fServoValue[LIMBS_LEG_LEFT][LEG_FORWARD_BACKWARD] 		= 10;
	fServoValue[LIMBS_LEG_LEFT][LEG_KNEE] 					= 10;
	fServoValue[LIMBS_LEG_LEFT][LEG_FOOT_FORWARD_BACKWARD]	= 10;
	fServoValue[LIMBS_LEG_LEFT][LEG_FOOT_RIGHT_LEFT] 		= 10;

	for (uint8_t u8LimpCounter = 0; u8LimpCounter < 4U; u8LimpCounter++)
	{
		for (uint8_t u8ServoCounter = 0; u8ServoCounter < 5U; u8ServoCounter++)
		{
			fHomePosition[u8LimpCounter][u8ServoCounter] = 80;
		}
	}
}

float Device::fGetServoValue(uint8_t u8Limp, uint8_t u8Servo)
{
	return this->fServoValue[u8Limp][u8Servo];
}

void Device::MessageCallback()
{
	oConnect.vReceive();
}

uint8_t Device::u8InitDevice()
{
	oConnect.vInit(CLIENT_BODY_CONTROLLER);
	return 0;
}


uint8_t Device::u8GetButtonBuffer()
{
	return 0;
}

void Device::vConnection()
{
	//Receive
	static uint16_t u16Counter = 0;
	static uint8_t u8ConnectedWithCache = 0;
	oConnect.vSendMessage();
	uTransmitReceive_t Receive;

	if (oConnect.u8GetNewMessage(Receive) == BUFFER_OK)
	{
		switch (Receive.uDataStatus.u8Mode)
		{
		case Mode_StatusMassage:
			if (Receive.uDataStatus.u8State == State_Ready)
			{
				u8ConnectedWithCache = u8ConnectedWithCache | Receive.uDataStatus.u8Sender;
			}
		break;
		case Mode_NewServoValue:
			u8SetServoValue(Receive.uDataServo.u8Limp,Receive.uDataServo.u8Servo,Receive.uDataServo.fData);
		break;
		}
	}

	//Transmit
	if (u16Counter % 40 == 0) //all 200ms
	{
		oConnect.vTransmitState();
	}
	u16Counter ++;

	//Set connenction
	if (u16Counter > 200)	//all sek
	{
		u16Counter = 0;
		oConnect.vSetConnectedWith(u8ConnectedWithCache);
		u8ConnectedWithCache = 0;
	}

	oConnect.vErrorHandling();
}

/* This method checks the buttons have been pressed and
 * writes it to the queue: ButtonQueueHandle
 *
 * Return:  BUFFER_OK ; BUFFER_FULL
 * Yven Vogt 15.04.2021 */
uint8_t Device::u8UpdateButtons()
{
	return 0;
}

void Device::vSetNewHomePosition(void)
{
	for (uint8_t u8LimpCounter = 0U; u8LimpCounter < 4U; u8LimpCounter++)
	{
		for (uint8_t u8ServoCounter = 0U; u8ServoCounter < 5U; u8ServoCounter++)
		{
			fHomePosition[u8LimpCounter][u8ServoCounter] = fServoValue[u8LimpCounter][u8ServoCounter];
		}
	}
}

/* This method checks the joystick for changes
 *
 * Return:  DEVICE_OK ; DEVICE_JOYSTICK_NO_NEW_VALUE
 * Yven Vogt 30.04.2021 */


/* This method checks the angles and changes: fServoValue
 *
 * Return:  DEVICE_OK ; DEVICE_SERVO_LIMIT
 * Yven Vogt 29.04.2021 */
uint8_t Device::u8SetServoValue(uint8_t u8Limp, uint8_t u8Servo, float fAddValue)
{
	uint8_t u8Return = DEVICE_OK;

	fServoValue[u8Limp][u8Servo] = fAddValue;

	if (fServoValue[u8Limp][u8Servo] < DEVICE_SERVO_MIN_ANGEL)
	{
		fServoValue[u8Limp][u8Servo] = DEVICE_SERVO_MIN_ANGEL;
		u8Return = DEVICE_SERVO_LIMIT;
	}
	if (fServoValue[u8Limp][u8Servo] > DEVICE_SERVO_MAX_ANGEL)
	{
		fServoValue[u8Limp][u8Servo] = DEVICE_SERVO_MAX_ANGEL;
		u8Return = DEVICE_SERVO_LIMIT;
	}
	return u8Return;
}

/* This method updates all connected peripherals
 *
 * Return:  void
 * Yven Vogt 15.04.2021 */
void Device::vUpdateDevice()
{
	u8UpdateButtons();
	u8GoToHomePosition();
}

/* This method answers whether the robot is allowed to move
 *
 * Return:  true ; false
 * Yven Vogt 29.04.2021 */
bool Device::bGetMovementFlag(void)
{
	return this->bMovementFlag;
}

uint8_t Device::u8GoToHomePosition(void)
{
	uint8_t u8Return = DEVICE_OK;
	uint8_t u8NotDone = 1U;
	if ((bGoToHomeFlag != false)&&(bMovementFlag == true))
	{
		u8Return = DEVICE_OK;
		float fDifference = 0.0F;
		u8NotDone = 0U;

		for (uint8_t u8LimpCounter = 0U; u8LimpCounter < 4U; u8LimpCounter++)
		{
			for (uint8_t u8ServoCounter = 0U; u8ServoCounter < 5U; u8ServoCounter++)
			{
				fDifference = fServoValue[u8LimpCounter][u8ServoCounter] - fHomePosition[u8LimpCounter][u8ServoCounter];
				if (fDifference > 1.0F)
				{
					u8SetServoValue(u8LimpCounter, u8ServoCounter, -1.0F);
					u8NotDone++;
				}
				else if (fDifference < (-1.0F))
				{
					u8SetServoValue(u8LimpCounter, u8ServoCounter, 1.0F);
					u8NotDone++;
				}
				else
				{
					fServoValue[u8LimpCounter][u8ServoCounter] = fHomePosition[u8LimpCounter][u8ServoCounter];
				}
			}
		}
	}
	if (u8NotDone == 0U)
	{
		bGoToHomeFlag = false;
	}

	return u8Return;
}

/* This method sets the MovementFlag
 *
 * Return:  true ; false
 * Yven Vogt 29.04.2021 */
bool Device::bSetMovementFlag(bool bNewFlagState)
{
	bool bReturn = false;
	if (bNewFlagState != bMovementFlag)
	{
		bMovementFlag = bNewFlagState;
		bReturn = true;
	}
	return bReturn;
}

bool Device::bSetGoToHomeFlag(bool bNewFlagState)
{
	bool bReturn = false;
	if (bNewFlagState != bGoToHomeFlag)
	{
		bGoToHomeFlag = bNewFlagState;
		bReturn = true;
	}
	return bReturn;
}

bool Device::bGetGoToHomeFlag(void)
{
	return bGoToHomeFlag;
}

//Device Device::instance;
//auto& oDevice = Device::getInstance();
Device oDevice;

