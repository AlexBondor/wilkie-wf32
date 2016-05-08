// Ensure this library description is only included once
#ifndef List_h
#define List_h

#include "Arduino.h"
#include <stdlib.h>

struct Node
{
  int id;
  double value;
  struct Node* next;
};

class List
{
  public:
    List();
    void add(int id, double value);
    void removeFirst();
    void updateFirstValue(double newValue);
    int getFirstId();
    double getFirstValue();

  private:
    int _nodesCount;
    struct Node* _head;
    struct Node* _tail;
};
#endif
