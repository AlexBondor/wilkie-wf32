#include "Robot.h"

Robot robot;

void setup()
{  
  // Serial1 is used by Bluetooth module to receive data
  Serial1.begin(9600);
  Serial.begin(19200);
  Serial.println("hello");

 // robot.moveForward(50);

 // robot.connectToWifi("dd-wrt", "qaz123wsx");
 // robot.connectToServer("192.168.1.128", 44300);
//  int size;
//  char** networks = robot.wifiScan(size);
//  for (int i = 0; i < size; i++)
//  {
//    Serial.println(networks[i]);
//  }
}

void loop()
{
  //robot.writeToServer("10", 2);
  robot.process();
  // Serial.print(robot.getLeftEyeData().getX());
  // Serial.print(" ");
  // Serial.println(robot.getLeftEyeData().getY());
  // Serial.println(robot.getLeftEyeRawData());
  // Serial.println("-----------------------");
  // delay(35);
  //delay(100);
  // Time to go now..
  // There is so much code I need to write.
  // Thank you for checking out the video!
}

