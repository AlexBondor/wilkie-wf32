// Ensure this library description is only included once
#ifndef ByteList_h
#define ByteList_h

#include "Arduino.h"
#include <stdlib.h>

struct Node
{
  byte* message;
  int length;
  struct Node* next;
};

class ByteList
{
  public:
    ByteList();
    void add(byte* message, int length);
    void removeFirst();
    byte* getFirstMessage();
    int getFirstLength();

  private:
    int _nodesCount;
    struct Node* _head;
    struct Node* _tail;
};
#endif
