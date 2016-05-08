// Ensure this library description is only included once
#ifndef Point_h
#define Point_h

#include "Arduino.h"

class Point 
{
  public:
    Point();
    double getX();
    void setX(double x);
    double getY();
    void setY(double y);

  private:
    double _x;
    double _y;
};

#endif
