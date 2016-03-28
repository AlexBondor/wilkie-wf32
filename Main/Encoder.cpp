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
	interruptPinValue = digitalRead(encoders[ENCODER_1_INDEX].interruptPin);
	digitalPinValue = digitalRead(encoders[ENCODER_1_INDEX].digitalPin);

	interruptPinValue ^ digitalPinValue ? encoders[ENCODER_1_INDEX].position-- : encoders[ENCODER_1_INDEX].position++;

	clearIntFlag(INTERRUPT_PIN_TO_EXTERNAL_IRQ(encoders[ENCODER_1_INDEX].interruptPin)); // Now that you've serviced the interrupt, clear
                                                                      								 // the interrupt flag so it doesn't get called
                                                                      								 // twice.
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
  interruptPinValue = digitalRead(encoders[ENCODER_2_INDEX].interruptPin);
  digitalPinValue = digitalRead(encoders[ENCODER_2_INDEX].digitalPin);

  interruptPinValue ^ digitalPinValue ? encoders[ENCODER_2_INDEX].position++ : encoders[ENCODER_2_INDEX].position--;

  clearIntFlag(INTERRUPT_PIN_TO_EXTERNAL_IRQ(encoders[ENCODER_2_INDEX].interruptPin)); // Now that you've serviced the interrupt, clear
                                                                                       // the interrupt flag so it doesn't get called
                                                                                       // twice. 
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
volatile int Encoder::getPosition()
{
	return encoders[_id].position;
}
