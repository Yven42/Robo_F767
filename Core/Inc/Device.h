/*
 * Device.h
 *
 *  Created on: Apr 15, 2021
 *      Author: Yven Vogt
 */

#ifndef APPLICATION_USER_CORE_DEVICE_H_
#define APPLICATION_USER_CORE_DEVICE_H_

#define DEVICE_ERROR 0U
#define DEVICE_OK 1U

#define BUTTON_BUFFER_SIZE 16U
#define BUFFER_FULL 1U
#define BUFFER_EMPTY 255U
#define BUFFER_OK 0U

#define DEVICE_JOYSTICK_GRID 400.0F
#define DEVICE_JOYSTICK_MIN_DIFFERENCE 0.1F
#define DEVICE_JOYSTICK_MIDDLE_POSITION 2050.0F
#define DEVICE_SERVO_MAX_STEP 3.0F
#define DEVICE_SERVO_MIN_STEP (-3.0F)
#define DEVICE_SERVO_MAX_ANGEL 160.0F
#define DEVICE_SERVO_MIN_ANGEL 0.0F
#define DEVICE_SERVO_LIMIT 0U
#define DEVICE_JOYSTICK_NO_NEW_VALUE 0U

#define LIMBS_ARM_RIGHT 0U
#define LIMBS_ARM_LEFT 1U
#define LIMBS_LEG_RIGHT 2U
#define LIMBS_LEG_LEFT 3U

#define ARM_SHOULDER_FORWARD 0U
#define ARM_SHOULDER_UP 1U
#define ARM_ELBOW 2U
#define ARM_HAND_ROTATE 3U
#define ARM_HAND_OPEN 4U

#define LEG_UP_DOWN 0U
#define LEG_FORWARD_BACKWARD 1U
#define LEG_KNEE 2U
#define LEG_FOOT_FORWARD_BACKWARD 3U
#define LEG_FOOT_RIGHT_LEFT 4U

#include "main.h"
#include "cmsis_os.h"
#include "Connection.h"

enum Limps
{
	Arm_Right  = 0,
    Arm_Left = 1,
	Leg_Right = 2,
	Leg_Left = 3
};



class Device
{
	public:
	    Device();
		uint8_t u8GetButtonBuffer(void);
		uint8_t u8UpdateButtons(void);
		void vUpdateDevice(void);
		uint8_t u8InitDevice(void);
		bool bGetMovementFlag(void);
		bool bSetMovementFlag(bool bNewFlagState);
		bool bSetGoToHomeFlag(bool bNewFlagState);
		bool bGetGoToHomeFlag(void);
		void vConnection();
		enum Limps eSelected_Limps;
		float fGetServoValue(uint8_t u8Limp, uint8_t u8Servo);
		uint8_t u8SetServoValue(uint8_t u8Limp, uint8_t u8Servo, float fAddValue);
		uint8_t u8GoToHomePosition(void);
		void vSetNewHomePosition(void);
		Connect oConnect;


	private:

		bool bMovementFlag = false;
		bool bGoToHomeFlag = false;
		float fServoValue[4][5];
		float fHomePosition[4][5];
};




#endif /* APPLICATION_USER_CORE_DEVICE_H_ */
