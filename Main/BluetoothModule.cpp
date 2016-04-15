#include "BluetoothModule.h"

char val = 0;

BluetoothModule::BluetoothModule()
{
  _last = 0;
  _isEnabled = false;
}

void BluetoothModule::listen()
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
      }
    }  
    _last = val;
  }
}

void BluetoothModule::setEnableRemoteControlCallbackMethod(VoidCallbackMethod callback)
{
  _enableRemoteControlCallbackMethod = callback;
}

void BluetoothModule::setMoveForwardCallbackMethod(VoidCallbackMethod callback)
{
  _moveForwardCallbackMethod = callback;
}

void BluetoothModule::setMoveBackwardCallbackMethod(VoidCallbackMethod callback)
{
  _moveBackwardCallbackMethod = callback;
}

void BluetoothModule::setTurnLeftCallbackMethod(VoidCallbackMethod callback)
{
  _turnLeftCallbackMethod = callback;
}

void BluetoothModule::setTurnRightCallbackMethod(VoidCallbackMethod callback)
{
  _turnRightCallbackMethod = callback;
}

void BluetoothModule::setEnablePumpCallbackMethod(VoidCallbackMethod callback)
{
  _enablePumpCallbackMethod = callback;
}

