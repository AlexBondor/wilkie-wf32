// Ensure this library description is only included once
#ifndef MotorController_h
#define MotorController_h

#include "Arduino.h"
#include "FuzzyController.h"
#include "Motor.h"
#include "Encoder.h"
#include "Config.h"
#include "Point.h"
#include "List.h"

// Circle length (cm) described by the robots' wheels 
// when turning in place
// Given that the distance between wheel is 20.8 cm
#define ROTATION_CIRCUMFERENCE 65.312

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
