/***********************************************
 *
 * File: nodeEventOn.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTON_H
#define NODEEVENTON_H

#include "nodeEvent.h"

class nodeEventOn : public nodeEvent
{
public:
    nodeEventOn(quint64 startTime, node *eventNode, double* coords);
    virtual void action();
    virtual bool check() { return true; }

private:
    double* m_coords;
};

#endif // NODEEVENTON_H
