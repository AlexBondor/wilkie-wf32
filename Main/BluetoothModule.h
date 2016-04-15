#ifndef BluetoothModule_h
#define BluetoothModule_h

#include "Arduino.h"
#include "Config.h"

// Bluetooth module uses Serial1 on pins 39, 40 on WF32
    
class BluetoothModule
{
  public:
    BluetoothModule();
    void listen();
    void setEnableRemoteControlCallbackMethod(VoidCallbackMethod);
    void setMoveForwardCallbackMethod(VoidCallbackMethod);
    void setMoveBackwardCallbackMethod(VoidCallbackMethod);
    void setTurnLeftCallbackMethod(VoidCallbackMethod);
    void setTurnRightCallbackMethod(VoidCallbackMethod);
    void setEnablePumpCallbackMethod(VoidCallbackMethod);
    
  private:
    int _last;
    bool _isEnabled;
    VoidCallbackMethod _enableRemoteControlCallbackMethod;
    VoidCallbackMethod _enablePumpCallbackMethod;
    VoidCallbackMethod _moveForwardCallbackMethod;
    VoidCallbackMethod _moveBackwardCallbackMethod;
    VoidCallbackMethod _turnLeftCallbackMethod;
    VoidCallbackMethod _turnRightCallbackMethod;
};

#endif
