/***********************************************
 *
 * File: nodeEvent.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "nodeEvent.h"

nodeEvent::nodeEvent(quint64 startTime, node *eventNode)
    :event(startTime)
{
    this->eventNode = eventNode;
}
