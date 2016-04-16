// Ensure this library description is only included once
#ifndef Robot_h
#define Robot_h

#include "Arduino.h"
#include "Controller/MotorController.h"
#include "Controller/BluetoothController.h"

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
    void startVacuum();
    void stopVacuum();
    void brake();

  private:
    void resetFlags();
    MotorController _motorController;
    BluetoothController _bluetoothController;

    // Callbacks called by bluetooth module
    static void enableRemoteControlCallback();
    static void enablePumpCallback();
    static void moveForwardCallback();
    static void moveBackwardCallback();
    static void turnLeftCallback();
    static void turnRightCallback();

    void updateMovement();
};
#endif
