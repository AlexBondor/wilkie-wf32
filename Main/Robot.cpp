// Include this library's description file
#include "Robot.h"

static Robot* instance;
static bool _enableRemoteControlFlag;
static bool _moveForwardFlag;
static bool _moveBackwardFlag;
static bool _turnLeftFlag;
static bool _turnRightFlag;
static bool _enablePumpFlag;
static bool _enableServoFreeMoveFlag;


void Robot::process()
{
  _motorController.process();
  _sensorController.process();
  _bluetoothController.listen();
  _wifiController.process();
}

// Callback methods called by bluetooth module
void Robot::enableRemoteControlCallback()
{
  instance->resetFlags();
  _enableRemoteControlFlag = !_enableRemoteControlFlag;
  instance->updateMovement();
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

void Robot::enableServoFreeMoveCallback()
{
  _enableServoFreeMoveFlag = !_enableServoFreeMoveFlag;
  if (_enableServoFreeMoveFlag)
  {
    instance->startSensorsServo();
  }
  else
  {
    instance->stopSensorsServo();
  }
}

/*
 * Robot constructor
 */
Robot::Robot()
{
  instance = this;
  _enableRemoteControlFlag = false;
  _moveForwardFlag = false;
  _moveBackwardFlag = false;
  _turnLeftFlag = false;
  _turnRightFlag = false;
  _enablePumpFlag = false;
  _enableServoFreeMoveFlag = false;
  
  // Initialize motor controller
  _motorController.init();

  // Set the callback methods for when the bluetooth module receives data
  _bluetoothController.setEnableRemoteControlCallbackMethod(enableRemoteControlCallback);
  _bluetoothController.setMoveForwardCallbackMethod(moveForwardCallback);
  _bluetoothController.setMoveBackwardCallbackMethod(moveBackwardCallback);
  _bluetoothController.setTurnLeftCallbackMethod(turnLeftCallback);
  _bluetoothController.setTurnRightCallbackMethod(turnRightCallback);
  _bluetoothController.setEnablePumpCallbackMethod(enablePumpCallback);
  _bluetoothController.setEnableServoFreeMoveCallbackMethod(enableServoFreeMoveCallback);
}

Point Robot::getPosition()
{
  return _motorController.getPosition();
}

double Robot::getHeading()
{
  return _motorController.getHeading();
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
    moveForward(10);
  }
  if (_moveBackwardFlag)
  {
    moveBackward(10);    
  }
  if (_turnLeftFlag)
  {
    turnLeft(90);
  }
  if (_turnRightFlag)
  {
    turnRight(90);
  }
}

/*
 *  Return a list of scanned networks
 */
char** Robot::wifiScan(int &size)
{
  return _wifiController.scan(size);
}
    
bool Robot::connectToWifi(char* wifiName, char* password)
{
  return _wifiController.connectToWifi(wifiName, password);
}

bool Robot::connectToServer(char* serverIp, int serverPort)
{
  return _wifiController.connectToServer(serverIp, serverPort);
}

bool Robot::writeToServer(char* messageToWrite, int messageToWriteSize)
{
	return _wifiController.writeToServer(messageToWrite, messageToWriteSize);
}

/*
 *  Sensors controller methods
 */
void Robot::startSensorsServo()
{
  _sensorController.setServoFreeMove(true);
}

void Robot::stopSensorsServo()
{
  _sensorController.setServoFreeMove(false);
}

float Robot::getLeftEyeRawData()
{
  return _sensorController.getSharpSensor1RawData();
}

Point Robot::getLeftEyeData()
{
  return _sensorController.getSharpSensor1Data();
}

float Robot::getFrontEyeRawData()
{
  return _sensorController.getSharpSensor2RawData();
}

Point Robot::getFrontEyeData()
{
  return _sensorController.getSharpSensor2Data();
}

float Robot::getRightEyeRawData()
{
  return _sensorController.getSharpSensor3RawData();
}

Point Robot::getRightEyeData()
{
  return _sensorController.getSharpSensor3Data();
}