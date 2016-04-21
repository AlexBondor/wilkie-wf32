#include "Robot.h"

Robot robot;

char xs1[10], ys1[10];
char xs2[10], ys2[10];
char xs3[10], ys3[10];

int start;

void setup()
{  
  // Serial1 is used by Bluetooth module to receive data
  Serial1.begin(9600);
  Serial.begin(19200);
  Serial.println("hello");
  robot.connectToWifi("dd-wrt", "qaz123wsx");
  robot.connectToServer("192.168.1.128", 44300);
//  int size;
//  char** networks = robot.wifiScan(size);
//  for (int i = 0; i < size; i++)
//  {
//    Serial.println(networks[i]);
//  }
  start = millis();
}

void loop()
{
  //robot.writeToServer("10", 2);
  robot.process();
  sprintf(xs1, "%d", (int)robot.getLeftEyeData().getX());
  sprintf(ys1, "%d", (int)robot.getLeftEyeData().getY());
  sprintf(xs2, "%d", (int)robot.getFrontEyeData().getX());
  sprintf(ys2, "%d", (int)robot.getFrontEyeData().getY());
  sprintf(xs3, "%d", (int)robot.getRightEyeData().getX());
  sprintf(ys3, "%d", (int)robot.getRightEyeData().getY());
  if (millis() - start > 100)
  {
    start = millis();
    robot.writeToServer(xs1, strlen(xs1));
    robot.writeToServer(ys1, strlen(ys1));
    robot.writeToServer(xs2, strlen(xs2));
    robot.writeToServer(ys2, strlen(ys2));
    robot.writeToServer(xs3, strlen(xs3));
    robot.writeToServer(ys3, strlen(ys3));
  }
  // delay(20);
  // delay(100);
  // Serial.print(robot.getLeftEyeData().getX());
  // Serial.print(" ");
  // Serial.println(robot.getLeftEyeData().getY());
  // Serial.println("-----------------------");
  // Serial.print(robot.getFrontEyeData().getX());
  // Serial.print(" ");
  // Serial.println(robot.getFrontEyeData().getY());
  // Serial.println("-----------------------");
  // Serial.print(robot.getRightEyeData().getX());
  // Serial.print(" ");
  // Serial.println(robot.getRightEyeData().getY());
  // Serial.println("+++++++++++++++++++++++");
}

