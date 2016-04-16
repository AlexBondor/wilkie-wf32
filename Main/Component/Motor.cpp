// Include this library's description file
#include "Motor.h"

/*
 * Empty constructor
 */
Motor::Motor()
{
}

/*
 * Updates the fields of current motor and sets the direction
 * of its pins
 */
void Motor::attach(int directionPinA, int directionPinB, int pwmPin)
{
  _directionPinA = directionPinA;
  _directionPinB = directionPinB;
  _pwmPin = pwmPin;

  pinMode(_directionPinA, OUTPUT);
  pinMode(_directionPinB, OUTPUT);
  pinMode(_pwmPin, OUTPUT);  
}

/*  
*   Turns the motor in a desired direction
*   forward/backward with a desired speed
*/
void Motor::turn(int speed)
{
  if (speed >= 0) 
  {
    _inA = HIGH;
  }
  else
  {
    _inA = LOW;
    speed = -speed;
  }
	// Adjust speed so it won't exceed imposed limits by PWM
	speed < 0 ? speed = 0 : speed > 255 ? speed = 255 : speed = speed;

	digitalWrite(_directionPinA, _inA);
	digitalWrite(_directionPinB, !_inA);
	analogWrite(_pwmPin, speed);
}

/*
*   Brakes the current motor
*/
void Motor::brake()
{
	digitalWrite(_directionPinA, LOW);
	digitalWrite(_directionPinB, LOW);
	analogWrite(_pwmPin, 0);
}

