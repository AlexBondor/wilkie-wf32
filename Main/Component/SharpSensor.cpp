#include "SharpSensor.h"

SharpSensor::SharpSensor()
{
  _readingsCount = 0;
  _lastIndex = 0;
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

/*
 *  Returns the distance in cm if the sensor has
 *  anything blocking its viewing ray. Returns 0 otherwise.
 */
float SharpSensor::getDistance()
{
  _readings[_readingsCount] = getRaw();
  _readingsCount += 1;
  if (_readingsCount >= _avg)
  {
    sort(_readings);
    _lastIndex = _readings[_avg / 2] - MIN_RAW;
    _readingsCount = 0;
  }
  
  return _lastIndex < 0 ? 0 : RAW_TO_CM[_lastIndex][1] + DISTANCE_FROM_SENSOR_TO_ROBOT_CENTER;
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

