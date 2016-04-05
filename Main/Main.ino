#include "Config.h"
#include "Motor.h"
#include "Encoder.h"
#include "FuzzyController.h"

Motor leftMotor;
Motor rightMotor;

Encoder leftEncoder;
Encoder rightEncoder;

FuzzyController fuzzyController;

long start;

void setup()
{
  // Attach motors
  leftMotor.attach(LEFT_MOTOR_IN_A_PIN, LEFT_MOTOR_IN_B_PIN, LEFT_MOTOR_PWM_PIN);
  rightMotor.attach(RIGHT_MOTOR_IN_A_PIN, RIGHT_MOTOR_IN_B_PIN, RIGHT_MOTOR_PWM_PIN);
  
  // Attach encoders
  leftEncoder.attach(LEFT_ENCODER_INT_PIN, LEFT_ENCODER_DIGITAL_PIN);
  rightEncoder.attach(RIGHT_ENCODER_INT_PIN, RIGHT_ENCODER_DIGITAL_PIN);
  
  Serial.begin(115200);
  start = millis();
}

void loop()
{
  if (millis() - start > 3000)
  {
    leftMotor.brake();
    rightMotor.brake();
    Serial.print("Left distance: ");
    Serial.print(leftEncoder.getPosition());
    Serial.print("Right distance: ");
    Serial.println(rightEncoder.getPosition());
    while(1);
  }
  int rin = rightEncoder.getSpeed();
  fuzzyController.setInput(1, rin);
  fuzzyController.fuzzify();
  int rout = fuzzyController.defuzzify(1);
  
  int lin = leftEncoder.getSpeed();
  fuzzyController.setInput(1, lin);
  fuzzyController.fuzzify();
  int lout = fuzzyController.defuzzify(1);
  
  leftMotor.turn(lout);
  rightMotor.turn(rout);

  Serial.print("lin: ");
  Serial.print(lin);
  Serial.print(" lout: ");
  Serial.print(lout);
  Serial.print(" rin: ");
  Serial.print(rin);
  Serial.print(" rout: ");
  Serial.println(rout);
}

