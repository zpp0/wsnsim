/***********************************************
 *
 * File: nodeEventRadioRX.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTRADIORX_H
#define NODEEVENTRADIORX_H

#include "QVector"

#include "nodeEvent.h"
#include "message.h"
#include "luaHandler.h"
#include "logevent.h"

class nodeEventRadioRX : public nodeEvent
{
public:
    nodeEventRadioRX(quint64 startTime, node *eventNode, message* mes, double RSSI);
    virtual void action();

    virtual bool check();

    static quint64 CollCount;
    static quint64 RXCount;

private:
    message* m_mes;       //сообщение, которое было принято
    double m_rssi;
    
    static LogEvent event;
};

#endif // NODEEVENTRADIORX_H
