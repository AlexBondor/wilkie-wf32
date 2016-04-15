// Include this library's description file
#include "Robot.h"

static Robot* instance;
static bool _enableRemoteControlFlag;
static bool _enablePumpFlag;
static bool _moveForwardFlag;
static bool _moveBackwardFlag;
static bool _turnLeftFlag;
static bool _turnRightFlag;

// Callback methods called by bluetooth module
void Robot::enableRemoteControlCallback()
{
  instance->resetFlags();
  _enableRemoteControlFlag = !_enableRemoteControlFlag;
  instance->updateMovement();
}

void Robot::enablePumpCallback()
{
  _enablePumpFlag = !_enablePumpFlag;
  if (_enablePumpFlag)
  {
    instance->startVacuum();
  }
  else
  {
    instance->stopVacuum();
  }
}

void Robot::moveForwardCallback()
{
  instance->resetFlags();
  _moveForwardFlag = !_moveForwardFlag;
  instance->updateMovement();
}

void Robot::moveBackwardCallback()
{
  instance->resetFlags();
  _moveBackwardFlag = !_moveBackwardFlag;
  instance->updateMovement();
}

void Robot::turnLeftCallback()
{
  instance->resetFlags();
  _turnLeftFlag = !_turnLeftFlag;
  instance->updateMovement();
}

void Robot::turnRightCallback()
{
  instance->resetFlags();
  _turnRightFlag = !_turnRightFlag;
  instance->updateMovement();
}

/*
 * Robot constructor
 */
Robot::Robot()
{
  instance = this;
  _enableRemoteControlFlag = false;
  _enablePumpFlag = false;
  _moveForwardFlag = false;
  _moveBackwardFlag = false;
  _turnLeftFlag = false;
  _turnRightFlag = false;
  
  // Initialize motor controller
  _motorController.init();

  // Set the callback methods for when the bluetooth module receives data
  _bluetoothController.setEnableRemoteControlCallbackMethod(enableRemoteControlCallback);
  _bluetoothController.setEnablePumpCallbackMethod(enablePumpCallback);
  _bluetoothController.setMoveForwardCallbackMethod(moveForwardCallback);
  _bluetoothController.setMoveBackwardCallbackMethod(moveBackwardCallback);
  _bluetoothController.setTurnLeftCallbackMethod(turnLeftCallback);
  _bluetoothController.setTurnRightCallbackMethod(turnRightCallback);
}

Point Robot::getPosition()
{
  return _motorController.getPosition();
}

void Robot::process()
{
  _motorController.process();
  _bluetoothController.listen();
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

void Robot::brake()
{
  _motorController.brake();
}

void Robot::startVacuum()
{
  _motorController.startVacuum();
}

void Robot::stopVacuum()
{
  _motorController.stopVacuum();
}

void Robot::resetFlags()
{
  _moveForwardFlag = false;
  _moveBackwardFlag = false;
  _turnLeftFlag = false;
  _turnRightFlag = false;  
}

/*
 * Updates the motion of the robot taking into account
 * the flags changed by the bluetooth module upron data 
 * receival
 */
void Robot::updateMovement()
{
  // Only one command executed at once
  _motorController.brake();
  if (_moveForwardFlag)
  {
    moveForward(100);
  }
  if (_moveBackwardFlag)
  {
    moveBackward(100);    
  }
  if (_turnLeftFlag)
  {
    turnLeft(360);
  }
  if (_turnRightFlag)
  {
    turnRight(360);
  }
}

