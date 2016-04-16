#include "List.h"

List::List()
{
  _head = NULL;
  _tail = NULL;
  _nodesCount = 0;  
}

/* 
 * Add a node at the end of the list 
 */
void List::add(int id, double value)
{
  struct Node *temp;
  temp = (struct Node *)malloc(sizeof(struct Node));
  temp->id = id;
  temp->value = value;
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
void List::removeFirst()
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
int List::getFirstId()
{
  if (_nodesCount != 0)
  {
    return _head->id;
  }
  return -1;
}

/*
 * Return the value of the first node
 */
double List::getFirstValue()
{
  if (_nodesCount != 0)
  {
    return _head->value;
  }
  return -1;
}

/*
 * Update the value of the first node
 */
void List::updateFirstValue(double newValue)
{
  if (_head != NULL)
  {
    _head->value = newValue;
  }
}

