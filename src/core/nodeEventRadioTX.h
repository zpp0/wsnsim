/***********************************************
 *
 * File: nodeEventRadioTX.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTRADIOTX_H
#define NODEEVENTRADIOTX_H

#include "QVector"

#include "nodeEvent.h"
#include "node.h"
#include "message.h"
#include "luaHandler.h"

/*
  класс описывает все события, которые происходят на узле
  и инициируются получением сообщения по радио
  */

class nodeEventRadioTX : public nodeEvent
{
public:
    nodeEventRadioTX(quint64 startTime, node *eventNode, message* mes, double rssi);
    virtual void action();
    virtual bool check() { return true; }

private:
    message* m_mes;       //сообщение, которое было принято
    double m_rssi;
    
    static LogEvent event;
};

#endif // NODEEVENTRADIOTX_H
