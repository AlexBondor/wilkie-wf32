// Ensure this library description is only included once
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor
{
  public:
    Motor();
    void attach(int, int, int);
    void turn(int, int);
    void brake();
    enum direction
	{
	  FORWARD,
	  BACKWARD
	};

  private:
  	int _directionPinA;
  	int _directionPinB;
  	int _pwmPin;
  	int _inA;
};
#endif
