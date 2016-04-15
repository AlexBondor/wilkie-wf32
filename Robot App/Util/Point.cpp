#include "Point.h"

Point::Point()
{
  // Initialize point to 0, 0
  setX(0);
  setY(0);
}

double Point::getX()
{
  return _x;
}

void Point::setX(double x)
{
  _x = x;
}

double Point::getY()
{
  return _y;
}

void Point::setY(double y)
{
  _y = y;
}

