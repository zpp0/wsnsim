/***********************************************
 *
 * File: nodeEventTimer.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODEEVENTTIMER_H
#define NODEEVENTTIMER_H

#include <QString>

#include "node.h"
#include "nodeEvent.h"
#include "logevent.h"

/*
  класс описывает все события, которые происходят на узле
  и инициируются внутренним прерыванием на узле (по таймеру)
  */

class nodeEventTimer : public nodeEvent
{
public:
    nodeEventTimer(quint64 startTime, node *eventNode, QString type);
    /* virtual ~nodeEventTimer(){}; */

    virtual void action();
    virtual bool check();

private:
    static LogEvent event;
    
    QString m_type;
};

#endif // NODEEVENTTIMER_H
