#include "Robot.h"
#include <Wire.h>

Robot robot;

/**
 *  If this value is changed then the value
 *  on the iic master must be changed as well
 */
#define IIC_MESSAGE_LENGTH 32

char posX[5], 
     posY[5],
     headDeg[5],
     xs1[5], 
     ys1[5],
     xs2[5], 
     ys2[5],
     xs3[5], 
     ys3[5],
     message[IIC_MESSAGE_LENGTH];

char* padding;
int messageLen = 0;

int start;

/*
 *  Gather data from encoders and sensors and send as
 *  IIC data when requested from iicController
 */
void iicSendData();
void initPaddingString();

void setup()
{  

  // IIC communication
  // Wire.begin(IIC_SLAVE_ADDRESS);
  // Wire.onRequest(iicSendData);

  // initPaddingString();

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
  if (millis() - start > 50)
  {
    start = millis();
    sprintf(xs1, "l%d", (int)robot.getLeftEyeData().getX());
    sprintf(ys1, "l%d", (int)robot.getLeftEyeData().getY());
    sprintf(xs2, "f%d", (int)robot.getFrontEyeData().getX());
    sprintf(ys2, "f%d", (int)robot.getFrontEyeData().getY());
    sprintf(xs3, "r%d", (int)robot.getRightEyeData().getX());
    sprintf(ys3, "r%d", (int)robot.getRightEyeData().getY());
    sprintf(posX, "p%d", (int)robot.getPosition().getX());
    sprintf(posY, "p%d", (int)robot.getPosition().getY());
    sprintf(headDeg, "h%d", (int)robot.getHeading());
    robot.writeToServer(xs1, strlen(xs1));
    robot.writeToServer(ys1, strlen(ys1));
    robot.writeToServer(xs2, strlen(xs2));
    robot.writeToServer(ys2, strlen(ys2));
    robot.writeToServer(xs3, strlen(xs3));
    robot.writeToServer(ys3, strlen(ys3));
    robot.writeToServer(posX, strlen(posX));
    robot.writeToServer(posY, strlen(posY));
    robot.writeToServer(headDeg, strlen(headDeg));
    robot.writeToServer(headDeg, strlen(headDeg));
  }
}

void iicSendData()
{
  sprintf(posX, "%d", (int)robot.getPosition().getX());
  sprintf(posY, "%d", (int)robot.getPosition().getY());
  sprintf(headDeg, "%d", (int)robot.getHeading());
  sprintf(xs1, "%d", (int)robot.getLeftEyeData().getX());
  sprintf(ys1, "%d", (int)robot.getLeftEyeData().getY());
  sprintf(xs2, "%d", (int)robot.getFrontEyeData().getX());
  sprintf(ys2, "%d", (int)robot.getFrontEyeData().getY());
  sprintf(xs3, "%d", (int)robot.getRightEyeData().getX());
  sprintf(ys3, "%d", (int)robot.getRightEyeData().getY());

  messageLen = 
    strlen(posX) +
    strlen(posY) +
    strlen(headDeg) +
    strlen(xs1) +
    strlen(ys1) +
    strlen(xs2) +
    strlen(ys2) +
    strlen(xs3) +
    strlen(ys3) +
    8; // represents the number of commas in between the data

  sprintf(message, "%s,%s,%s,%s,%s,%s,%s,%s,%s%*.*s", posX, posY, headDeg, xs1, ys1, xs2, ys2, xs3, ys3, IIC_MESSAGE_LENGTH - messageLen, IIC_MESSAGE_LENGTH - messageLen, padding);

  Serial.println(message);

  Wire.write(message);
}

void initPaddingString()
{
  padding = (char*) malloc(IIC_MESSAGE_LENGTH * sizeof(char));
  for (int i = 0; i < IIC_MESSAGE_LENGTH; i++)
  {
    padding[i] = ',';
  }
}