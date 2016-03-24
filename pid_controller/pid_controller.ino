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

#define DEBOUNCING_TIME_US 10000

volatile int leftInterruptPinValue = 0;
volatile int leftDigitalPinValue = 0;
volatile int rightInterruptPinValue = 0;
volatile int rightDigitalPinValue = 0;

volatile int leftPosition = 0;
volatile int rightPosition = 0;

unsigned long leftLastInterrupt = 0;
unsigned long rightLastInterrupt = 0;

volatile bool led1State = false, led2State = false;

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

// This is our Interrupt Service Routine
// It has to be defined before getting called, so we put it up front here.
void __attribute__((interrupt)) leftInterruptRoutine()
{
  if ((micros() - leftLastInterrupt) >= DEBOUNCING_TIME_US)
  {
    leftInterruptPinValue = digitalRead(leftInterruptPin);
    leftDigitalPinValue = digitalRead(leftDigitalPin);
  
    leftInterruptPinValue ^ leftDigitalPinValue ? leftPosition-- : leftPosition++;

    clearIntFlag(_EXTERNAL_1_IRQ); // Now that you've serviced the interrupt, clear
                                   // the interrupt flag so it doesn't get called
                                   // twice.
    leftLastInterrupt = micros();
    led1State = !led1State;
    digitalWrite(PIN_LED1, led1State);
  }
}

// This is our Interrupt Service Routine
// It has to be defined before getting called, so we put it up front here.
void __attribute__((interrupt)) rightInterruptRoutine()
{
  if ((micros() - rightLastInterrupt) >= DEBOUNCING_TIME_US)
  {
    rightInterruptPinValue = digitalRead(rightInterruptPin);
    rightDigitalPinValue = digitalRead(rightDigitalPin);
    
    rightInterruptPinValue ^ rightDigitalPinValue ? rightPosition-- : rightPosition++;
    
    clearIntFlag(_EXTERNAL_2_IRQ); // Now that you've serviced the interrupt, clear
                                 // the interrupt flag so it doesn't get called
                                 // twice.
    rightLastInterrupt = micros();
    led2State = !led2State;
    digitalWrite(PIN_LED2, led2State);
  }
}

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

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);


  // Initialize interrupts
  
  setIntVector(_EXTERNAL_1_VECTOR, leftInterruptRoutine); // Set the ISR vector to our
                                               // LEDchange function. That way,
                                               // when Int1 gets triggered, we
                                               // execute leftInterruptRoutine.
  setIntPriority(_EXTERNAL_1_VECTOR, 4, 0); // Set the priority of Int1 to 4. Not
                                            // too high, not too low.
  clearIntFlag(_EXTERNAL_1_IRQ); // Clear the flag. Always do this before enabling
                                 // the interrupt.
  setIntEnable(_EXTERNAL_1_IRQ); // Enable the interrupt. (Have you cleared the
                                 // flag first? Good. Just checking.)
  
  setIntVector(_EXTERNAL_2_VECTOR, rightInterruptRoutine); // Set the ISR vector to our
                                               // LEDchange function. That way,
                                               // when Int2 gets triggered, we
                                               // execute rightInterruptRoutine.
  setIntPriority(_EXTERNAL_2_VECTOR, 4, 0); // Set the priority of Int2 to 4. Not
                                            // too high, not too low.
  clearIntFlag(_EXTERNAL_2_IRQ); // Clear the flag. Always do this before enabling
                                 // the interrupt.
  setIntEnable(_EXTERNAL_2_IRQ); // Enable the interrupt. (Have you cleared the
                                 // flag first? Good. Just checking.)

  pinMode(PIN_INT1, INPUT); // Set the pin for Int1 to input.

  pinMode(PIN_INT2, INPUT); // Set the pin for Int2 to input.

  Serial.begin(115200);
}

void loop() 
{
  turnMotor(leftMotor, forward, 50);
  turnMotor(rightMotor, forward, 50);
  delay(2000);
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

  tempA = motorDirection == forward ? HIGH : LOW;
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

