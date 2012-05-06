/***********************************************
 *
 * File: nodeEventRadioRXBegin.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTRADIORXBEGIN_H
#define NODEEVENTRADIORXBEGIN_H

#include "QVector"

#include "nodeEvent.h"
#include "message.h"
#include "luaHandler.h"

class nodeEventRadioRXBegin : public nodeEvent
{
public:
    nodeEventRadioRXBegin(quint64 startTime, node *eventNode, message* mes, double RSSI);
    virtual void action();
    virtual bool check();

private:
    message* m_mes;       //сообщение, которое было принято
    double m_rssi;
};

#endif // NODEEVENTRADIORXBEGIN_H
