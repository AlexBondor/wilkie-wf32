#include "Robot.h"

Robot robot;

void setup()
{  
  robot.moveForward(20);
  robot.turnRight(90);
  robot.moveBackward(10);
  robot.turnLeft(90);
}

void loop()
{
  robot.process();
}

