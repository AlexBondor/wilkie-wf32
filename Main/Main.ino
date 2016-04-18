#include "Robot.h"

Robot robot;

void setup()
{  
  // Serial1 is used by Bluetooth module to receive data
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("hello");

  // robot.connectToWifi("dd-wrt", "qaz123wsx");
  // robot.connectToServer("192.168.1.128", 44300);
  int size;
  char** networks = robot.wifiScan(size);
  for (int i = 0; i < size; i++)
  {
    Serial.println(networks[i]);
  }
}

void loop()
{
  // robot.writeToServer(buff, s);
  // robot.process();

  // Time to go now..
  // There is so much code I need to write.
  // Thank you for checking out the video!
}

