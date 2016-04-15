#include "Robot.h"

Robot robot;
  
void setup()
{  
  //robot.moveForward(200);
  //robot.startVacuum();

  // Serial1 is used by Bluetooth module to receive data
  Serial1.begin(9600);
}

void loop()
{
  robot.process();

  // Time to go now..
  // There is so much code I need to write.
  // Thank you for checking out the video!
}

