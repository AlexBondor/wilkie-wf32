#include "Config.h"
#include "Motor.h"
#include "Encoder.h"

Motor leftMotor;
Motor rightMotor;

Encoder leftEncoder;
Encoder rightEncoder;

void setup()
{
  // Attach motors
  leftMotor.attach(LEFT_MOTOR_IN_A_PIN, LEFT_MOTOR_IN_B_PIN, LEFT_MOTOR_PWM_PIN);
  rightMotor.attach(RIGHT_MOTOR_IN_A_PIN, RIGHT_MOTOR_IN_B_PIN, RIGHT_MOTOR_PWM_PIN);
  
  // Attach encoders
  leftEncoder.attach(LEFT_ENCODER_INT_PIN, LEFT_ENCODER_DIGITAL_PIN);
  rightEncoder.attach(RIGHT_ENCODER_INT_PIN, RIGHT_ENCODER_DIGITAL_PIN);
  
  Serial.begin(115200);
}

void loop()
{
  Serial.print("left: ");
  Serial.print(leftEncoder.getPosition());
  Serial.print(" right: ");
  Serial.println(rightEncoder.getPosition());
  leftMotor.turn(Motor::FORWARD, 50);
  rightMotor.turn(Motor::FORWARD, 50);
  delay(1);
}

