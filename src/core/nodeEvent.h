/***********************************************
 *
 * File: nodeEvent.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENT_H
#define NODEEVENT_H

#include "event.h"
#include "node.h"

/*
  класс отвечает за все события, связанные с узлами
  */
class nodeEvent : public event
{
public:
    nodeEvent(quint64 startTime, node* eventNode);
    virtual ~nodeEvent(){};

    /* virtual void action(); */
    /* virtual bool check(); */
    
    //ссылка на узел, с которым происходит событие
    node* eventNode;
};

#endif // NODEEVENT_H
