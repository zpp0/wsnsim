/***********************************************
 *
 * File: nodeEventMovement.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTMOVEMENT_H
#define NODEEVENTMOVEMENT_H

#include <QString>

#include "node.h"
#include "nodeEvent.h"

class nodeEventMovement : public nodeEvent
{
public:
    nodeEventMovement(quint64 startTime, node *eventNode, double* newCoord);
    /* virtual ~nodeEventMovement(){}; */

    virtual void action();
    virtual bool check();

private:
    double* m_coord;
};

#endif // NODEEVENTMOVEMENT_H
