// Ensure this library description is only included once
#ifndef MotorController_h
#define MotorController_h

#include "Arduino.h"
#include "../Controller/FuzzyController.h"
#include "../Component/Motor.h"
#include "../Component/Encoder.h"
#include "../Config.h"
#include "../Util/Point.h"
#include "../Util/List.h"

// Circle length (cm) described by the robots' wheels 
// when turning in place
// Given that the distance between wheel is 20.5 cm
#define ROTATION_CIRCUMFERENCE 64.4
#define ROTATION_RADIUS 10.25
#define RAD_TO_DEG 57.296

#define MOVE_FORWARD 0
#define MOVE_BACKWARD 1
#define TURN_RIGHT 2
#define TURN_LEFT 3

class MotorController
{
  public:
    MotorController();
    void init();
    Point getPosition();
    Point getDirection();
    void process();
    void moveForward(double distance);
    void moveBackward(double distance);
    void turnLeft(int degrees);
    void turnRight(int degrees);

    void startVacuum();
    void stopVacuum();
    
    void brake();

  private:
    List _commands;    
    int _commandsCount;
    Point _position;
    Point _direction;
    bool _processingNewCommand;
    
    double _leftMotorPosition;
    double _rightMotorPosition;
    double _leftMotorSpeed;
    double _rightMotorSpeed;
    int _leftMotorPWM;
    int _rightMotorPWM;

    // Motion related methods
    void doBrake();
    void doMoveForward();
    void doMoveBackward();
    void doTurnLeft();
    void doTurnRight();
    void fuzzyCompute();
    void fuzzyMove(double left, double right);
};
#endif
