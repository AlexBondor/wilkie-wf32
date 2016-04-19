#ifndef SensorController_h
#define SensorController_h

#include "Arduino.h"
#include <Servo.h>
#include "../Component/SharpSensor.h"
#include "../Config.h"
#include "../Util/Point.h"

// Starting position of the servo motor
#define ZERO_POSITION 105
// Angle in degrees the motor turns in both directions
#define ROTATION_ANGLE 60

class SensorController
{
	public:
		SensorController();
		void process();

		void setServoTurnDelay(int newTurnDelay);
		int getServoPosition();
		void setServoPosition(int newPosition);
		void setServoFreeMove(bool freeMove);

		float getSharpSensor1RawData();
		Point getSharpSensor1Data();
		float getSharpSensor2RawData();
		Point getSharpSensor2Data();
		float getSharpSensor3RawData();
		Point getSharpSensor3Data();

	private:
		Servo _myServo;
		bool _freeMove;

		int _myServoDirection;

		int _startTime;
		int _myServoTurnDelay; // Servo step delay in milliseconds
		int _myServoPosition;

		SharpSensor _sharpSensor1;
		SharpSensor _sharpSensor2;
		SharpSensor _sharpSensor3;
};

#endif