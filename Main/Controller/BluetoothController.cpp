#include "BluetoothController.h"

char val = 0;

BluetoothController::BluetoothController()
{
  _last = 0;
  _isEnabled = false;
}

/*
 *  This method must be called every loop cycle in
 *  order for the Bluetooth module to react to new
 *  commands.
 */
void BluetoothController::listen()
{
  if (Serial1.available() > 0)
  {
    val = Serial1.read();

    if (val == 10) // New line character
    {
      _last = _last - 48; 

      if (_last == 0)
      {
        _isEnabled = !_isEnabled;
        _enableRemoteControlCallbackMethod();
      }
      if (_isEnabled)
      {
        if (_last == 1)
        {
          _moveForwardCallbackMethod();
        }
        if (_last == 2)
        {
          _moveBackwardCallbackMethod();
        }
        if (_last == 3)
        {
          _turnLeftCallbackMethod();
        }
        if (_last == 4)
        {
          _turnRightCallbackMethod();
        }
        if (_last == 5)
        {
          _enablePumpCallbackMethod();
        }
        if (_last == 6)
        {
          _enableServoFreeMoveCallbackMethod();
        }
      }
    }  
    _last = val;
  }
}

void BluetoothController::setEnableRemoteControlCallbackMethod(VoidCallbackMethod callback)
{
  _enableRemoteControlCallbackMethod = callback;
}

void BluetoothController::setMoveForwardCallbackMethod(VoidCallbackMethod callback)
{
  _moveForwardCallbackMethod = callback;
}

void BluetoothController::setMoveBackwardCallbackMethod(VoidCallbackMethod callback)
{
  _moveBackwardCallbackMethod = callback;
}

void BluetoothController::setTurnLeftCallbackMethod(VoidCallbackMethod callback)
{
  _turnLeftCallbackMethod = callback;
}

void BluetoothController::setTurnRightCallbackMethod(VoidCallbackMethod callback)
{
  _turnRightCallbackMethod = callback;
}

void BluetoothController::setEnablePumpCallbackMethod(VoidCallbackMethod callback)
{
  _enablePumpCallbackMethod = callback;
}

void BluetoothController::setEnableServoFreeMoveCallbackMethod(VoidCallbackMethod callback)
{
  _enableServoFreeMoveCallbackMethod = callback;
}