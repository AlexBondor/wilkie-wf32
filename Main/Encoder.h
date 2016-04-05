// Ensure this library description is only included once
#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

#define MAX_ENCODERS 2

typedef struct 
{
  int interruptPin;
  int digitalPin;
  volatile int position;
  volatile int speed;
  volatile long lastInterruptMicros;
} encoder_t;

class Encoder
{
  public:
    Encoder();
    void attach(int, int);
    volatile int getPosition();
    volatile int getSpeed();

  private:
    void interruptInitialSetup(int);
    int _id;
};
#endif
