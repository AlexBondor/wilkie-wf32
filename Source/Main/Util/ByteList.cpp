#include "ByteList.h"

ByteList::ByteList()
{
  _head = NULL;
  _tail = NULL;
  _nodesCount = 0;  
}

/* 
 * Add a node at the end of the list 
 */
void ByteList::add(byte* message, int length)
{
  struct Node *temp;
  temp = (struct Node *)malloc(sizeof(struct Node));
  temp->message = message;
  temp->length = length;
  temp->next = NULL;
  
  if (_head == NULL)
  {
    _head = temp;
    _tail = _head;
  }
  else
  {
    _tail->next = temp;
    _tail = temp;
  }
  _nodesCount ++;
}
/*
 * Remove first node of the list
 */
void ByteList::removeFirst()
{
  if (_nodesCount == 0)
  {
    return;
  }
  
  struct Node *temp;
  temp = _head->next;

  free(_head);
  _head = temp;

  _nodesCount --;
}

/*
 * Return the id of the first node
 */
byte* ByteList::getFirstMessage()
{
  if (_nodesCount != 0)
  {
    return _head->message;
  }
  return NULL;
}

/*
 * Return the value of the first node
 */
int ByteList::getFirstLength()
{
  if (_nodesCount != 0)
  {
    return _head->length;
  }
  return -1;
}

