/***********************************************
 *
 * File: nodeEventMeasure.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENMEASURE_H
#define NODEEVENMEASURE_H

#include <QString>

#include "node.h"
#include "nodeEvent.h"

/*
  */

class nodeEventMeasure : public nodeEvent
{
public:
    nodeEventMeasure(quint64 startTime, node* eventNode, quint16 m_sensorID, double value);
    virtual ~nodeEventMeasure(){};
    
    virtual void action();
    virtual bool check() { return true; }

private:
    quint16 m_sensorID;
    double m_value;
};

#endif // NODEEVENMEASURE_H
