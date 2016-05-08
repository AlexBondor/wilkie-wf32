#include "Robot.h"
#include <Wire.h>

Robot robot;

/**
 *  If this value is changed then the value
 *  on the iic master must be changed as well
 */
#define IIC_MESSAGE_LENGTH 32

char vacF[5],
     svoF[5],
     noCmds[5],
     posX[5], 
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
void iicRecvData(int numBytes);
void initPaddingString();

void setup()
{  
  // Serial1 is used by Bluetooth module to receive data
  Serial1.begin(9600);
  Serial.begin(19200);
  Serial.println("hello");

  // IIC communication
  Wire.begin(IIC_SLAVE_ADDRESS);
  Wire.onRequest(iicSendData);
  Wire.onReceive(iicRecvData);

  initPaddingString();

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
  sprintf(noCmds, "%d", robot.getNumberOfCommands());
  sprintf(vacF, "%d", robot.getVacuumStatus());
  sprintf(svoF, "%d", robot.getSensorsServoStatus());
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
    strlen(noCmds) +
    strlen(vacF) +
    strlen(svoF) +
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

  sprintf(message, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s%*.*s", noCmds, vacF, svoF, posX, posY, headDeg, xs1, ys1, xs2, ys2, xs3, ys3, IIC_MESSAGE_LENGTH - messageLen, IIC_MESSAGE_LENGTH - messageLen, padding);

  //Serial.println(message);

  Wire.write(message);
}

int cmd = 0;
char amount[5];
int amountIndex = 0;

/*
 *  Commands list:
 *  10 - no operation
 *  20 - brake 
 *  31 - move forward
 *  32 - move backward
 *  43 - turn left
 *  44 - turn right
 *  55 - toggle vacuum
 *  56 - toggle sensors free move 
 */
void executeCommandFirst()
{
  Serial.println(cmd);
  switch(cmd)
  {
    case 10:
      break;
    case 20:
      robot.brake();
      break;
    case 31:
      robot.moveForward(atoi(amount));
      break;
    case 32:
      robot.moveBackward(atoi(amount));
      break;
    case 43:
      robot.turnLeft(atoi(amount));
      break;
    case 44:
      robot.turnRight(atoi(amount));
      break;
    case 55:
      Serial.println("toggle vacuum");
      robot.toggleVacuum();
      break;
    case 56:
      Serial.println("toggle servo");
      robot.toggleSensorsServo();
      break;
    default:
      robot.brake();
      break;
  }
}

/*
 *  Receive commands from master Zybo
 *  Commands types:
 *    Actions:
 *      m - move
 *      t - turn
 *      n - no operation
 *      s - stop
 *      p - power
 *    Directions:
 *      f - forward
 *      b - backward
 *      l - left
 *      r - right
 *    Amount:
 *      XXX - a number represented with digits
 *    Devices:
 *      v - vacuum
 *      o - servo      
 *
 *  Example of valid commands from master:
 *  mf10 - move forward 10 cm
 *  tl45 - turn left 45 cm
 *  s    - stop; robot brakes
 *  n    - no operation; robot proceeds normally
 *  pv   - power vacuum
 */
void iicRecvData(int numBytes)
{
  char x;
  while(Wire.available() != 0)
  {
    x = Wire.read();
    switch(x)
    {
      case 'n':
        if (cmd != 0)
        {
          executeCommandFirst();
        }
        cmd = 10;
        amountIndex = 0;
        break;
      case 's':
        if (cmd != 0)
        {
          executeCommandFirst();
        }
        cmd = 20;
        amountIndex = 0;
        break;
      case 'm':
        if (cmd != 0)
        {
          executeCommandFirst();
        }
        cmd = 30;
        amountIndex = 0;
        break;
      case 't':
        if (cmd != 0)
        {
          executeCommandFirst();
        }
        cmd = 40;
        amountIndex = 0;
        break;
      case 'p':
        if (cmd != 0)
        {
          executeCommandFirst();
        }
        cmd = 50;
        amountIndex = 0;
        break;
      case 'f':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 1;
        break;
      case 'b':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 2;
        break;
      case 'l':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 3;
        break;
      case 'r':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 4;
        break;
      case 'v':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 5;
        break;
      case 'o':
        if (cmd % 10 != 0)
        {
          executeCommandFirst();
        }
        cmd += 6;
        break;
      default:
        if (x - '0' >= 0 && x - '0' <= 9)
        {
          amount[amountIndex++] = x;
        }
        else
        {
          cmd = 20; // No known operation
        }
        break;
    }
  }
}

void initPaddingString()
{
  padding = (char*) malloc(IIC_MESSAGE_LENGTH * sizeof(char));
  for (int i = 0; i < IIC_MESSAGE_LENGTH; i++)
  {
    padding[i] = ',';
  }
}