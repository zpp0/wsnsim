/***********************************************
 *
 * File: nodeEventLocalTime.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTLOCALTIME_H
#define NODEEVENTLOCALTIME_H

#include "QVector"

#include "nodeEvent.h"
#include "luaHandler.h"

class nodeEventLocalTime : public nodeEvent
{
public:
    nodeEventLocalTime(quint64 startTime, node *eventNode, quint64 localtime);
    virtual void action();
    virtual bool check();

private:
    quint64 m_localTime;
};

#endif // NODEEVENTLOCALTIME_H
