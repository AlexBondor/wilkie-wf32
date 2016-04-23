#include "Robot.h"

Robot robot;

char px[10], py[10];
char h[10];
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
  robot.process();
  if (millis() - start > 100)
  {
    start = millis();
    sprintf(xs1, "%d", (int)robot.getLeftEyeData().getX());
    sprintf(ys1, "%d", (int)robot.getLeftEyeData().getY());
    sprintf(xs2, "%d", (int)robot.getFrontEyeData().getX());
    sprintf(ys2, "%d", (int)robot.getFrontEyeData().getY());
    sprintf(xs3, "%d", (int)robot.getRightEyeData().getX());
    sprintf(ys3, "%d", (int)robot.getRightEyeData().getY());
    sprintf(px, "p%d", (int)robot.getPosition().getX());
    sprintf(py, "p%d", (int)robot.getPosition().getY());
    sprintf(h, "h%d", (int)robot.getHeading());
    sprintf(h, "h%d", (int)robot.getHeading());
    robot.writeToServer(xs1, strlen(xs1));
    robot.writeToServer(ys1, strlen(ys1));
    robot.writeToServer(xs2, strlen(xs2));
    robot.writeToServer(ys2, strlen(ys2));
    robot.writeToServer(xs3, strlen(xs3));
    robot.writeToServer(ys3, strlen(ys3));
    robot.writeToServer(px, strlen(px));
    robot.writeToServer(py, strlen(py));
    robot.writeToServer(h, strlen(h));
    robot.writeToServer(h, strlen(h));
  }
}

