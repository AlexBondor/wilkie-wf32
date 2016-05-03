// Include this library's description file
#include "MotorController.h"

Motor leftMotor;
Motor rightMotor;

Encoder leftEncoder;
Encoder rightEncoder;

FuzzyController fuzzyController;

//#define MOTOR_CONTROLLER_DEBUG

#ifdef MOTOR_CONTROLLER_DEBUG
#define DEBUG_PWM 70
#endif

/*
 * Empty constructor
 */
MotorController::MotorController()
{
}

/*
 * Updates the fields of current motor and sets the Heading
 * of its pins
 */
void MotorController::init()
{   
  // Attach motors
  leftMotor.attach(LEFT_MOTOR_IN_A_PIN, LEFT_MOTOR_IN_B_PIN, LEFT_MOTOR_PWM_PIN);
  rightMotor.attach(RIGHT_MOTOR_IN_A_PIN, RIGHT_MOTOR_IN_B_PIN, RIGHT_MOTOR_PWM_PIN);
  
  // Attach encoders
  leftEncoder.attach(LEFT_ENCODER_INT_PIN, LEFT_ENCODER_DIGITAL_PIN);
  rightEncoder.attach(RIGHT_ENCODER_INT_PIN, RIGHT_ENCODER_DIGITAL_PIN);  

  // Consider the starting point of the robot
  // as its origin
  _position.setX(0);
  _position.setY(0);

  // Suppose robot is heading along Y axis
  _heading = 90;

  // Initialize the number of commands to 0
  _commandsCount = 0;
  _processingNewCommand = true;
  
  _leftMotorPosition = 0;
  _leftMotorLastPosition = 0;
  _rightMotorPosition = 0;
  _rightMotorLastPosition = 0;
  _leftMotorSpeed = 0;
  _rightMotorSpeed = 0;
  _leftMotorPWM = 0;
  _rightMotorPWM = 0;

  // Set vacuum pump pin as output
  pinMode(VACUUM_PIN, OUTPUT);
  stopVacuum();
}

/*
 * This method must be called every loop so that the
 * first command from the commands list gets consumed
 */
void MotorController::process()
{
  // No commands to process
  if (_commandsCount == 0)
  {
    // Stop the robot
    doBrake();
    return;
  }

  // Get id and value of current command to be
  // processed
  int id = _commands.getFirstId();
  double value = _commands.getFirstValue();

  // Command successfully executed
  if (value <= 0)
  {
    _commands.removeFirst();
    _commandsCount--;
    doBrake();
    return;
  }

  // Make sure to reset encoder position just to 
  // have a good starting reference point
  if (_processingNewCommand)
  {
    _processingNewCommand = false;
    _leftMotorPosition = 0;
    _rightMotorPosition = 0;
  }
  
  switch(id)
  {
    case MOVE_FORWARD:
      doMoveForward();
      break;
    case MOVE_BACKWARD:
      doMoveBackward();
      break;
    case TURN_LEFT:
      doTurnLeft();
      break;
    case TURN_RIGHT:
      doTurnRight();
      break;
    default:
      doBrake();
      break;
  }
}

/*
 * Removes all the queued commands and stops
 * the robot
 */
void MotorController::brake()
{
  while(_commandsCount != 0)
  {
    _commands.removeFirst();
    _commandsCount--;
  }
  doBrake();
}

/*
 * Make the robot come to a full stop
 */
void MotorController::doBrake()
{
    _processingNewCommand = true;
    leftMotor.brake();
    rightMotor.brake();  
}

/*
 * Move the robot forward and update the value of the 
 * move forward command which happens to be the first
 * in the commands list, reason for being called
 */
void MotorController::doMoveForward()
{
  fuzzyMove(1, 1);

  _leftMotorPosition += leftEncoder.getPosition() - _leftMotorLastPosition;
  _rightMotorPosition += rightEncoder.getPosition() - _rightMotorLastPosition;

  updatePositionAndHeading(1);

  double average = (_leftMotorPosition + _rightMotorPosition) / 2;

  if (average >= _commands.getFirstValue())
  {
    _commands.updateFirstValue(0);
  }
}

/*
 * Move the robot backward and update the value of the 
 * move backward command which happens to be the first
 * in the commands list, reason for being called
 */
void MotorController::doMoveBackward()
{
  fuzzyMove(-1, -1);

  _leftMotorPosition += _leftMotorLastPosition - leftEncoder.getPosition();
  _rightMotorPosition += _rightMotorLastPosition - rightEncoder.getPosition();

  updatePositionAndHeading(2);

  double average = (_leftMotorPosition + _rightMotorPosition) / 2;

  if (average >= _commands.getFirstValue())
  {
    _commands.updateFirstValue(0);
  }
}

/*
 * Turn the robot to the left and update the value of the 
 * turn left command which happens to be the first
 * in the commands list, reason for being called
 */
void MotorController::doTurnLeft()
{
  fuzzyMove(-1, 1);

  // When moving in place half down the speed of the motors
  // as they compose because they're moving in different Headings
  _leftMotorPosition += leftEncoder.getPosition() - _leftMotorLastPosition;
  _rightMotorPosition += rightEncoder.getPosition() - _rightMotorLastPosition;

  updatePositionAndHeading(3);

  double average = (-_leftMotorPosition + _rightMotorPosition) / 2;
  double radians = average / ROTATION_RADIUS;
  double degrees = radians * RAD_TO_DEG;
  
  if (degrees >= _commands.getFirstValue())
  {
    _commands.updateFirstValue(0);
  }
}

/*
 * Turn the robot to the right and update the value of the 
 * turn right command which happens to be the first
 * in the commands list, reason for being called
 */
void MotorController::doTurnRight()
{
  // When moving in place half down the speed of the motors
  // as they compose because they're moving in different Headings
  fuzzyMove(1, -1);

  _leftMotorPosition += leftEncoder.getPosition() - _leftMotorLastPosition;
  _rightMotorPosition += rightEncoder.getPosition() - _rightMotorLastPosition;

  updatePositionAndHeading(4);

  double average = (_leftMotorPosition - _rightMotorPosition) / 2;
  double radians = average / ROTATION_RADIUS;
  double degrees = radians * RAD_TO_DEG;
  
  if (degrees >= _commands.getFirstValue())
  {
    _commands.updateFirstValue(0);
  }
}

void MotorController::moveForward(double distance)
{
  _commands.add(MOVE_FORWARD, distance);
  _commandsCount++;
}

void MotorController::moveBackward(double distance)
{
  _commands.add(MOVE_BACKWARD, distance);
  _commandsCount++;
}

void MotorController::turnLeft(int degrees)
{
  _commands.add(TURN_LEFT, degrees);
  _commandsCount++;
}

void MotorController::turnRight(int degrees)
{
  _commands.add(TURN_RIGHT, degrees);
  _commandsCount++;
}

/*
 *  Based on the already existing fuzzy sets and taking the
 *  encoders data as input, the fuzzy controller computes the
 *  new output that needs to be written on the motors.
 */
void MotorController::fuzzyCompute()
{

  #ifdef MOTOR_CONTROLLER_DEBUG
  _leftMotorSpeed = leftEncoder.getSpeed();
  _rightMotorSpeed = rightEncoder.getSpeed();
  Serial.print("Left motor speed: ");
  Serial.print(_leftMotorSpeed);
  Serial.print(" Right motor speed: ");
  Serial.println(_rightMotorSpeed);
  _leftMotorPWM = DEBUG_PWM;
  _rightMotorPWM = DEBUG_PWM;
  #else
  _leftMotorSpeed = leftEncoder.getSpeed();
  fuzzyController.setInput(1, _leftMotorSpeed);
  fuzzyController.fuzzify();
  _leftMotorPWM = fuzzyController.defuzzify(1);
  
  _rightMotorSpeed = rightEncoder.getSpeed();
  fuzzyController.setInput(1, _rightMotorSpeed);
  fuzzyController.fuzzify();
  _rightMotorPWM = fuzzyController.defuzzify(1);  
  #endif
}

void MotorController::fuzzyMove(double left, double right)
{
  fuzzyCompute();
  
  #ifdef MOTOR_CONTROLLER_DEBUG
  Serial.print("Left motor PWM: ");
  Serial.print(_leftMotorPWM);
  Serial.print(" Right motor PWM: ");
  Serial.println(_rightMotorPWM);
  Serial.println("");
  #endif
  leftMotor.turn(_leftMotorPWM * left);
  rightMotor.turn(_rightMotorPWM * right);
}

Point MotorController::getPosition()
{
  Point centerPosition;
  
  centerPosition.setX(_position.getX() + COS[(int)_heading] * WHEELS_CENTER_TO_ROBOT_CENTER_OFFSET);
  centerPosition.setY(_position.getY() + SIN[(int)_heading] * WHEELS_CENTER_TO_ROBOT_CENTER_OFFSET);

  return centerPosition;
}

double MotorController::getHeading()
{
  return _heading;
}

void MotorController::startVacuum()
{
  digitalWrite(VACUUM_PIN, HIGH);
}

void MotorController::stopVacuum()
{
  digitalWrite(VACUUM_PIN, LOW);
}

/*
 *  motionType parameter:
 *  1 - move forward
 *  2 - move backward
 *  3 - turn left
 *  4 - turn right
 */
void MotorController::updatePositionAndHeading(int motionType)
{
  double leftMotorPositionDelta = leftEncoder.getPosition() - _leftMotorLastPosition;
  double rightMotorPositionDelta = rightEncoder.getPosition() - _rightMotorLastPosition;

  _leftMotorLastPosition = leftEncoder.getPosition();
  _rightMotorLastPosition = rightEncoder.getPosition();

  double deltaPosition = (leftMotorPositionDelta + rightMotorPositionDelta) / 2;

  if (motionType == 1 || motionType == 2)
  {
    _position.setX(_position.getX() + deltaPosition * COS[round(_heading)]);
    _position.setY(_position.getY() + deltaPosition * SIN[round(_heading)]);

    // If robot turns slightly to left when moving forward or backward
    // if (leftMotorPositionDelta < rightMotorPositionDelta)
    // {
    //   double average = rightMotorPositionDelta - leftMotorPositionDelta;
    //   double radians = average / ROTATION_CIRCUMFERENCE;
    //   double degrees = radians * RAD_TO_DEG;   

    //   // _heading = motionType == 1 ? _heading + degrees : _heading - degrees;
    // }
    // else
    // {
    //   double average = leftMotorPositionDelta - rightMotorPositionDelta;
    //   double radians = average / ROTATION_CIRCUMFERENCE;
    //   double degrees = radians * RAD_TO_DEG;   

    //   // _heading = motionType == 1 ? _heading - degrees : _heading + degrees;
    // }
  }
  else
  {
    // Robot turns left
    if (motionType == 3)
    {
      double average = (rightMotorPositionDelta - leftMotorPositionDelta) / 2;
      double radians = average / ROTATION_RADIUS;
      double degrees = radians * RAD_TO_DEG;

      _heading += degrees;
    }
    else
    {
      double average = (leftMotorPositionDelta - rightMotorPositionDelta) / 2;
      double radians = average / ROTATION_RADIUS;
      double degrees = radians * RAD_TO_DEG;

      _heading -= degrees;
    }
  }
  _heading = _heading < 0 ? 360 + _heading : _heading;
  _heading = _heading > 360 ? _heading - 360 : _heading;
  Serial.println(_heading);
}
