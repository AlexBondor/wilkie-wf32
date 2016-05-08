// Include this library's description file
#include "Encoder.h"

/*
 * Returns the external vector specific to the interrupt
 * pin given as parameter
 */
#define INTERRUPT_PIN_TO_EXTERNAL_VECTOR(_interruptPin) \
	(_interruptPin == 2 ? _EXTERNAL_1_VECTOR : \
		_interruptPin == 7 ? _EXTERNAL_2_VECTOR : \
			_interruptPin == 8 ? _EXTERNAL_3_VECTOR : _EXTERNAL_0_VECTOR)

/*
 * Returns the external IRQ specific to the interrupt pin
 * given as parameter
 */
#define INTERRUPT_PIN_TO_EXTERNAL_IRQ(_interruptPin) \
	(_interruptPin == 2 ? _EXTERNAL_1_IRQ : \
		_interruptPin == 7 ? _EXTERNAL_2_IRQ : \
			_interruptPin == 8 ? _EXTERNAL_3_IRQ : _EXTERNAL_0_IRQ)

/*
 * Indexes from encoders list
 * 
 * Add more defines here if more encoders added
 */
#define ENCODER_1_INDEX 0
#define ENCODER_2_INDEX 1

/* 
 * List of encoder_t for better management 
 * 
 * encoders[0] references the encoder whose interrupt
 *    pin is connected to INT1 on WF32 board
 * encoders[1] references the encoder whose interrupt
 *    pin is connected to INT2 on WF32 board
 * ...
 * so on if more encoders added
 */
static encoder_t encoders[MAX_ENCODERS];

// Variables used by the interrupt routines
int interruptPinValue, digitalPinValue;

uint8_t _encoderCount = 0;

long lastInterruptMicros = 0;

/*
 * Interrupt routine for pin connected to INT1 on Chipkit WF32
 * 
 * The encoder has 2 pins used for detecting motion of the motor
 * but the position can be also calculated out of the values at the
 * moment of which the interrupt occurs.
 * 
 * Only 1 of the 2 pins of the encoder is connected to an interrupt
 * pin on Chipkit WF32 due to not so many interrupts available on the board
 */
void __attribute__((interrupt)) handleInterrupt1()
{  
  encoders[ENCODER_1_INDEX].timeBetweenInterrupts = micros() - encoders[ENCODER_1_INDEX].lastInterruptMicros;
  encoders[ENCODER_1_INDEX].lastInterruptMicros = micros();
  
	interruptPinValue = digitalRead(encoders[ENCODER_1_INDEX].interruptPin);
	digitalPinValue = digitalRead(encoders[ENCODER_1_INDEX].digitalPin);

	interruptPinValue ^ digitalPinValue ? encoders[ENCODER_1_INDEX].position-- : encoders[ENCODER_1_INDEX].position++;

	clearIntFlag(INTERRUPT_PIN_TO_EXTERNAL_IRQ(encoders[ENCODER_1_INDEX].interruptPin)); // Now that you've serviced the interrupt, clear
                                                                      								 // the interrupt flag so it doesn't get called
                                                                      								 // twice.

  if (encoders[ENCODER_1_INDEX].timeBetweenInterrupts <= 0)
  {
    return;
  }

  // Subtract the last reading
  encoders[ENCODER_1_INDEX].total -= encoders[ENCODER_1_INDEX].readings[encoders[ENCODER_1_INDEX].readIndex];
  // Save new data to readings vector
  encoders[ENCODER_1_INDEX].readings[encoders[ENCODER_1_INDEX].readIndex++] = encoders[ENCODER_1_INDEX].timeBetweenInterrupts;
  // Add new data to total value
  encoders[ENCODER_1_INDEX].total += encoders[ENCODER_1_INDEX].timeBetweenInterrupts;
  if (encoders[ENCODER_1_INDEX].readIndex == NUM_READINGS)
  {
    encoders[ENCODER_1_INDEX].readIndex = 0;
  }
  // Compute the average value
  encoders[ENCODER_1_INDEX].average = encoders[ENCODER_1_INDEX].total / NUM_READINGS;
}

/*
 * Interrupt routine for pin connected to INT2 on Chipkit WF32
 * 
 * The encoder has 2 pins used for detecting motion of the motor
 * but the position can be also calculated out of the values at the
 * moment of which the interrupt occurs.
 * 
 * Only 1 of the 2 pins of the encoder is connected to an interrupt
 * pin on Chipkit WF32 due to not so many interrupts available on the board
 */
void __attribute__((interrupt)) handleInterrupt2()
{  
  encoders[ENCODER_2_INDEX].timeBetweenInterrupts = micros() - encoders[ENCODER_2_INDEX].lastInterruptMicros;
  encoders[ENCODER_2_INDEX].lastInterruptMicros = micros();
  
  interruptPinValue = digitalRead(encoders[ENCODER_2_INDEX].interruptPin);
  digitalPinValue = digitalRead(encoders[ENCODER_2_INDEX].digitalPin);

  interruptPinValue ^ digitalPinValue ? encoders[ENCODER_2_INDEX].position++ : encoders[ENCODER_2_INDEX].position--;

  clearIntFlag(INTERRUPT_PIN_TO_EXTERNAL_IRQ(encoders[ENCODER_2_INDEX].interruptPin)); // Now that you've serviced the interrupt, clear
                                                                                       // the interrupt flag so it doesn't get called
                                                                                       // twice.

  if (encoders[ENCODER_2_INDEX].timeBetweenInterrupts < DINT_MIN_LIMIT || encoders[ENCODER_2_INDEX].timeBetweenInterrupts > DINT_MAX_LIMIT)
  {
    return;
  }

  // Subtract the last reading
  encoders[ENCODER_2_INDEX].total -= encoders[ENCODER_2_INDEX].readings[encoders[ENCODER_2_INDEX].readIndex];
  // Save new data to readings vector
  encoders[ENCODER_2_INDEX].readings[encoders[ENCODER_2_INDEX].readIndex++] = encoders[ENCODER_2_INDEX].timeBetweenInterrupts;
  // Add new data to total value
  encoders[ENCODER_2_INDEX].total += encoders[ENCODER_2_INDEX].timeBetweenInterrupts;
  if (encoders[ENCODER_2_INDEX].readIndex == NUM_READINGS)
  {
    encoders[ENCODER_2_INDEX].readIndex = 0;
  }
  
  // Compute the average value
  encoders[ENCODER_2_INDEX].average = encoders[ENCODER_2_INDEX].total / NUM_READINGS;
}

void Encoder::interruptInitialSetup(int interruptPin)
{
  int externalVector = INTERRUPT_PIN_TO_EXTERNAL_VECTOR(interruptPin);
  int externalIRQ = INTERRUPT_PIN_TO_EXTERNAL_IRQ(interruptPin);

  if(_id == ENCODER_1_INDEX)
  {
    setIntVector(externalVector, handleInterrupt1); // Set the ISR vector to our
                                                    // interruptRoutine function. That way,
                                                    // when int gets triggered, we
                                                    // execute interruptRoutine.
  }
  else
  {
    setIntVector(externalVector, handleInterrupt2); // Set the ISR vector to our
                                                    // interruptRoutine function. That way,
                                                    // when int gets triggered, we
                                                    // execute interruptRoutine.
  }

                          
	setIntPriority(externalVector, 4, 0); // Set the priority of Int1 to 4. Not
											  							   // too high, not too low.
	clearIntFlag(externalIRQ); // Clear the flag. Always do this before enabling
				    								 // the interrupt.
	setIntEnable(externalIRQ); // Enable the interrupt. (Have you cleared the
				    								 // flag first? Good. Just checking.)
}

/*
 * Add the new encoder to the static list of encoders 
 * for further reference
 */
Encoder::Encoder()
{
  _id = _encoderCount++;
  encoders[_id].position = 0;
  encoders[_id].timeBetweenInterrupts = 0;
  encoders[_id].lastInterruptMicros = 0;
}

/*
 * Updates the encoders list with the pins of the current encoder
 * and sets them as INPUT
 * 
 * Call the interruptInitialSetup function
 */
void Encoder::attach(int interruptPin, int digitalPin)
{
  encoders[_id].interruptPin = interruptPin;
  encoders[_id].digitalPin = digitalPin;
	pinMode(encoders[_id].interruptPin, INPUT);
  pinMode(encoders[_id].digitalPin, INPUT);   
  
  interruptInitialSetup(encoders[_id].interruptPin);
}

/*
 * Simply return the position of the encoder which changes when each
 * interrupt occurs
 */
volatile double Encoder::getPosition()
{
	return (encoders[_id].position / (INT_PER_REVOLUTION)) * WHEEL_CIRCUMFERENCE;
}

/*
 * Return the speed of the robot in RPM
 */
volatile int Encoder::getSpeed()
{
  // Robot stopped
  if (micros() - encoders[_id].lastInterruptMicros > encoders[_id].timeBetweenInterrupts + MAX_STALL_TIME)
  {
    return 0;  
  }
  if (encoders[_id].average != 0) {
    return DINT_TO_RMP / encoders[_id].average;
  }
  return 0;
}

/*
 * Set the position of current encoder to 0
 */
void Encoder::resetPosition()
{
  encoders[_id].position = 0;
}

