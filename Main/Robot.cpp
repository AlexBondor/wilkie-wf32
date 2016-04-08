// Include this library's description file
#include "Robot.h"

/*
 * Robot constructor
 */
Robot::Robot()
{
  // Initialize motor controller
  _motorController.init();
}

Point Robot::getPosition()
{
  return _motorController.getPosition();
}

void Robot::process()
{
  _motorController.process();
}

void Robot::moveForward(double distance)
{
  _motorController.moveForward(distance);
}

void Robot::moveBackward(double distance)
{
  _motorController.moveBackward(distance);
}

void Robot::turnLeft(int degrees)
{
  _motorController.turnLeft(degrees);
}

void Robot::turnRight(int degrees)
{
  _motorController.turnRight(degrees);
}

