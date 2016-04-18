#include "Robot.h"

Robot robot;
int cou = 10;
int s = 2;
char buff[5];

void setup()
{  
  //robot.moveForward(200);
  //robot.startVacuum();

  // Serial1 is used by Bluetooth module to receive data
  // Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("hello");

  robot.connectToWifi("dd-wrt", "qaz123wsx");
  robot.connectToServer("192.168.1.128", 44300);
}

void loop()
{
  if (cou > 999)
  {
    while(1)
    {
      robot.wifiProcess();
    }
  }
  if (cou > 99)
  {
    s = 3;
  }
  itoa(cou++, buff, 10);
  robot.wifiProcess();
  robot.writeToServer(buff, s);
  delay(10);
  // robot.process();

  // Time to go now..
  // There is so much code I need to write.
  // Thank you for checking out the video!
}

