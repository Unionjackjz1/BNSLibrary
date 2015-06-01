#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  RightEnc,       sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           leftMotor,     tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           rightMotor,    tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// VexSpecific_EventBus.c
//
// Author: Justin Marple with Team BNS
// Contact: jmarple@umass.edu
// Date: 05/31/2015
//
// This example file shows how to use
// the EventBus interface in BNSLib
// Note: The Task UpdateMotors() recieves
//   messages, but doesn't know (and doesn't
//   care) where those messages come from
//   This is why using an event bus creates
//   code that is loosely coupled.
//
// Dependencies:
//   BNSLib.h in previous directory
//
// ------------------------------------------------------------------------
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// The author can be contacted by email at jmarple@umass.edu
//
// ------------------------------------------------------------------------

#include "..\BNSLib.h"

#define LEFT_MOTOR 0x01
#define RIGHT_MOTOR 0x02


// ------------------------------------------------------------------------
// Update Motors Task
// This code is "isolated" from the rest of the code as it has no knowledge
// of whats going outside of it.  This allows for code at this level, or at
// higher levels to be moved around and edited alot easier.
task UpdateMotors
{
	// Subscribe to the message busses
	int leftMotorID = EventBusSubscribe(LEFT_MOTOR, int, 50);
	int rightMotorID = EventBusSubscribe(RIGHT_MOTOR, int, 50);

	// Default Motor Values
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;

	MotorControl pod1;
	MotorControlInit(&pod1, rightMotor, -1, -1, -1, RightEnc, SENSOR_IME);
	MotorControlSetPWM(&pod1, 0);
	MotorControlSetVelocityPID(&pod1, -1.0, 0, 0);

	while(1==1)
	{
		// Update the left motor if there was a new message
		int* pLeftMotor = EventBusGetMessage(leftMotorID);
		if(pLeftMotor != 0)
			motor[leftMotor] = *pLeftMotor;

		// Update the right motor if there was a new message
		int* pRightMotor = EventBusGetMessage(rightMotorID);
		if(pRightMotor != 0)
			MotorControlSetPWM(&pod1, *pRightMotor);

		MotorControlUpdate(&pod1);
		writeDebugStreamLine("%f, %f", pod1.motorPWM, RobotDataGetVelocitySummed(&pod1.sensorData));
		delay(10);
	}
}
// ------------------------------------------------------------------------



task main()
{
	// Initialize BNSLib and turn on the event bus
	BNS();
	BNSEventBus();

	// Start the task that prints messages out to the debug stream
	startTask(UpdateMotors);

	// Ramp up and down
	while(1==1)
	{
		for(int i = 0; i < 80; i++)
		{
			EventBusPublish(LEFT_MOTOR, &i);
			EventBusPublish(RIGHT_MOTOR, &i);
			delay(100);
		}

		for(int i = 80; i > 0; i--)
		{
			EventBusPublish(LEFT_MOTOR, &i);
			EventBusPublish(RIGHT_MOTOR, &i);
			delay(100);
		}
	}
}
