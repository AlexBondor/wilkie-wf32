#include "SensorController.h"

SensorController::SensorController()
{
	_myServoTurnDelay = 10;
	_myServoPosition = ZERO_POSITION;
	_freeMove = false;
	_myServoDirection = 1;

	_myServo.attach(SERVO_PIN);
	_myServo.write(_myServoPosition);

	_sharpSensor1.attach(SHARP_1_PIN, 20);
	_sharpSensor2.attach(SHARP_2_PIN, 20);
	_sharpSensor3.attach(SHARP_3_PIN, 20);
}

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
	else
	{
		_myServo.write(ZERO_POSITION);
	}
}

void SensorController::setServoFreeMove(bool freeMove)
{
	_freeMove = freeMove;
}

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
	_myServo.write(_myServoPosition);
}

float SensorController::getSharpSensor1Data()
{
	return _sharpSensor1.getDistance();
}

float SensorController::getSharpSensor2Data()
{
	return _sharpSensor2.getDistance();
}

float SensorController::getSharpSensor3Data()
{
	return _sharpSensor3.getDistance();
}
