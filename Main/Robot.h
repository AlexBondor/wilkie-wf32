// Ensure this library description is only included once
#ifndef Robot_h
#define Robot_h

#include "Arduino.h"
#include "MotorController.h"

class Robot
{
  public:
    Robot();
    void process();
    Point getPosition();
    void moveForward(double distance);
    void moveBackward(double distance);
    void turnLeft(int degrees);
    void turnRight(int degrees);

  private:
    MotorController _motorController;
};
#endif
