#include "SensorController.h"

SensorController::SensorController()
{
	_myServoTurnDelay = 20;
	_myServoPosition = ZERO_POSITION;
	_freeMove = false;
	_myServoDirection = 1;

	_myServo.attach(SERVO_PIN);
	_myServo.write(_myServoPosition);

	_sharpSensor1.attach(SHARP_1_PIN, 20);
	_sharpSensor2.attach(SHARP_2_PIN, 20);
	_sharpSensor3.attach(SHARP_3_PIN, 20);
}

/*
 *	This method must be called every loop cycle in order
 *	to make the servo on top of which the sensors are located.
 *	As in the case of other peripherals, we make the call each loop
 *	instead of waiting for it because we still want that other peripherals
 *	get the CPU as well.. Sharing is caring!
 */
void SensorController::process()
{
	if (_freeMove)
	{
		if (millis() - _startTime > _myServoTurnDelay)
		{
			_startTime = millis();

			if (_myServoPosition > ZERO_POSITION + ROTATION_ANGLE)
			{
				_myServoDirection = -1;
			}
			if (_myServoPosition < ZERO_POSITION - ROTATION_ANGLE)
			{
				_myServoDirection = 1;
			}

			_myServoPosition += _myServoDirection;
			setServoPosition(_myServoPosition);
		}		
	}
	_myServo.write(_myServoPosition);
}

void SensorController::setServoFreeMove(bool freeMove)
{
	_freeMove = freeMove;
	_startTime = millis();
}

/*
 *	Set the delay between servo steps in miliseconds
 */
void SensorController::setServoTurnDelay(int newTurnDelay)
{
	_myServoTurnDelay = newTurnDelay;
}

int SensorController::getServoPosition()
{
	return _myServoPosition;
}

void SensorController::setServoPosition(int newPosition)
{
	_myServoPosition = newPosition;
}
/*
 *	The distance returned by this method has the point of
 *	origin the position of the IR receiver from the sensor
 */
float SensorController::getSharpSensor1RawData()
{
	return _sharpSensor1.getDistance();
}

/*
 *	Return a point seen by this sensor where the points' origin of
 *	system of coordinates is the center of the robot
 */
Point SensorController::getSharpSensor1Data()
{
	Point myPoint;

	myPoint.setX(round(_sharpSensor1.getDistance() * COS[(_myServoPosition + 90 - NINETY_OFFSET) % 360]));
	myPoint.setY(round(_sharpSensor1.getDistance() * SIN[(_myServoPosition + 90 - NINETY_OFFSET) % 360]));
	return myPoint;
}

float SensorController::getSharpSensor2RawData()
{
	return _sharpSensor2.getDistance();
}

Point SensorController::getSharpSensor2Data()
{
	Point myPoint;

	myPoint.setX(round(_sharpSensor2.getDistance() * COS[(_myServoPosition - NINETY_OFFSET) % 360]));
	myPoint.setY(round(_sharpSensor2.getDistance() * SIN[(_myServoPosition - NINETY_OFFSET) % 360]));
	return myPoint;
}

float SensorController::getSharpSensor3RawData()
{
	return _sharpSensor3.getDistance();
}

Point SensorController::getSharpSensor3Data()
{
	Point myPoint;

	myPoint.setX(round(_sharpSensor3.getDistance() * COS[(_myServoPosition - 90 - NINETY_OFFSET) % 360]));
	myPoint.setY(round(_sharpSensor3.getDistance() * SIN[(_myServoPosition - 90 - NINETY_OFFSET) % 360]));
	return myPoint;
}