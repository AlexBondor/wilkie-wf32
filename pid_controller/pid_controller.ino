#include <SoftPWMServo.h>

// Motor encoder pins definitions
#define leftInterruptPin   2 // INT1
#define leftDigitalPin    28
#define rightInterruptPin  7 // INT2
#define rightDigitalPin   33

// Motor pin definitions
//
// InA  = InB - fast brake
// InA != InB - forward/backward direction
//
#define leftMotorInAPin   4 // direction pin
#define leftMotorInBPin  30 // direction pin
#define leftMotorEnPin    3 // enable pin used for PWM signal
#define rightMotorInAPin  5 // direction pin
#define rightMotorInBPin 31 // direction pin
#define rightMotorEnPin   6 // enable pin used for PWM signal

#define potentiometerPin 61

int leftInterruptPinValue = 0;
int leftDigitalPinValue = 0;
int rightInterruptPinValue = 0;
int rightDigitalPinValue = 0;

int leftPosition = 0;
int rightPosition = 0;

enum direction
{
  forward,
  backward
};

enum side 
{
  leftMotor,
  rightMotor
};

void setup()
{

  pinMode(leftInterruptPin, INPUT);
  pinMode(leftDigitalPin, INPUT);  
  pinMode(rightInterruptPin, INPUT);
  pinMode(rightDigitalPin, INPUT);  

  pinMode(leftMotorInAPin, OUTPUT);
  pinMode(leftMotorInBPin, OUTPUT);
  pinMode(leftMotorEnPin, OUTPUT);
  pinMode(rightMotorInAPin, OUTPUT);
  pinMode(rightMotorInBPin, OUTPUT);
  pinMode(rightMotorEnPin, OUTPUT);
  
  attachInterrupt(1, leftInterruptRoutine, CHANGE);
  attachInterrupt(2, rightInterruptRoutine, CHANGE);
  Serial.begin(9600);
}

void loop() 
{
  turnMotor(leftMotor, forward, 50);
  turnMotor(rightMotor, forward, 50);
  delay(20000);
  fastBrake(leftMotor);
  fastBrake(rightMotor);
  turnMotor(leftMotor, backward, 50);
  turnMotor(rightMotor, backward, 50);
  delay(2000);
  fastBrake(leftMotor);
  fastBrake(rightMotor);
}

void turnMotor(side selectedMotor, direction motorDirection, int speed) 
{
  speed < 0 ? speed = 0 : speed > 255 ? speed = 255 : speed = speed;
  int tempA, tempB;

  motorDirection == forward ? tempA = LOW : tempA = HIGH;
  tempB = !tempA;
  
  if (selectedMotor == leftMotor) 
  {
    digitalWrite(leftMotorInAPin, tempA);
    digitalWrite(leftMotorInBPin, tempB);
    analogWrite(leftMotorEnPin, speed);
  } 
  else 
  {
    digitalWrite(rightMotorInAPin, tempA);
    digitalWrite(rightMotorInBPin, tempB);  
    analogWrite(rightMotorEnPin, speed); 
  }  
}

void fastBrake(side selectedMotor) 
{
  if (selectedMotor == leftMotor) 
  {
    analogWrite(leftMotorEnPin, 0);
    digitalWrite(leftMotorInAPin, LOW);
    digitalWrite(leftMotorInBPin, LOW);
  } 
  else 
  {
    analogWrite(rightMotorEnPin, 0);
    digitalWrite(rightMotorInAPin, LOW);
    digitalWrite(rightMotorInBPin, LOW);    
  }
}

void leftInterruptRoutine () 
{
  leftInterruptPinValue = digitalRead(leftInterruptPin);
  leftDigitalPinValue = digitalRead(leftDigitalPin);
  
  leftInterruptPinValue ^ leftDigitalPinValue ? leftPosition-- : leftPosition++;
}

void rightInterruptRoutine () 
{
  rightInterruptPinValue = digitalRead(rightInterruptPin);
  rightDigitalPinValue = digitalRead(rightDigitalPin);
  
  rightInterruptPinValue ^ rightDigitalPinValue ? rightPosition-- : rightPosition++;
}

