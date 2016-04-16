#include "SharpSensor.h"

SharpSensor::SharpSensor()
{
  _readings = new int(50);
}

void SharpSensor::attach(int pin, int avg)
{
  _pin = pin;
  _avg = avg;  
  pinMode(_pin, INPUT);
}

int SharpSensor::getRaw()
{
  int raw = analogRead(_pin);

  if (raw < MIN_RAW || raw > MAX_RAW)
  {
    return 0;
  }

  return raw;
}

float SharpSensor::getDistance()
{
  _readingsCount = 0;

  for (int i = 0; i < _avg; i++)
  {
    _readings[_readingsCount++] = getRaw();
  }

  sort(_readings);

  int x = _readings[_avg / 2] - MIN_RAW;

  if (x < 0)
  {
    return 0;
  }

  return RAW_TO_CM[x][1];
}

void SharpSensor::sort(int* readings)
{
  int temp, swap;
  for (int i = 1 ; i < _avg; i++)
  {
    temp = i;

    while (temp > 0 && readings[temp] < readings[temp - 1])
    {
      swap             = readings[temp];
      readings[temp]   = readings[temp - 1];
      readings[temp - 1] = swap;

      temp--;
    }
  }
}

