// Ensure this library description is only included once
#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

#define MAX_ENCODERS 2

// Needed to obtain the RPM value given the time
// between consecutive interrupts in microseconds
//
// T_INT   - time between consecutive interrupts (in microseconds)
// T_EWRus - time the encoder wheel makes a rotation (in microseconds)
// T_EWRs  - time the encoder wheel makes a rotation (in seconds)
// T_WRs   - time the robot wheel makes a rotation (in seconds)
// N_WR/s  - number of robot wheel rotations per second
// RPM     - robot wheel rotations per minute
//
// Number obtained in the following way:
//
// T_EWRus = 3 * T_INT;         // 3 encoder wheel interrupts per encoder wheel rotation
// T_EWRs  = T_EWRus / 1000000; // 1s = 1000000us
// T_WRs   = 53 * T_EWRs;       // it takes 53 encoder wheel rotation for the robot wheel
//                              // to make one revolution (53:1 gear ratio)
// N_WR/s  = 1 / T_WRs
// RPM     = 60 * N_WR / s
//
// So.. 
//                    1 * 60
// DINT_TO_RPM =  --------------
//                T_INT * 3 * 53
//                 -----------
//                   1000000
//
#define DINT_TO_RMP 337358

// Lower bound of the data interval
// Represents ~250 RPM
#define DINT_MIN_LIMIT 1500
// Upper bound of the data interval
// Represents ~3 RPM
#define DINT_MAX_LIMIT 110000

// Used by filtering algorithm
#define NUM_READINGS 20
// Readings are made in us, so the noise band
// is expressed in us as well
#define NOISE_BAND 2000

// Use this time in microseconds to identify
// that the motor is not rotating anymore
#define MAX_STALL_TIME 20000 

// Number of encoder wheel interrupts for the robot wheel
// to make a revolution
#define INT_PER_REVOLUTION 159

// Robot wheel circumference in cm
#define WHEEL_CIRCUMFERENCE 10.05

typedef struct 
{
  int interruptPin;
  int digitalPin;
  volatile double position;
  volatile int timeBetweenInterrupts;
  volatile long lastInterruptMicros;
  
  // Filtering specific required variables and methods
  // Output needs to be filtered as the encoder readings
  // are not consistent
  int readings[NUM_READINGS];
  int readIndex = 0;
  int total = 0;
  int average = 0;
} encoder_t;

class Encoder
{
  public:
    Encoder();
    void attach(int, int);
    volatile double getPosition();
    volatile int getSpeed();
    void resetPosition();

  private:
    void interruptInitialSetup(int);
    int _id;
};
#endif
