/***********************************************
 *
 * File: nodeEventInner.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include <QString>
#include <QVector>

#include "api.h"
#include "nodeEventTimer.h"
#include "log.h"
#include "luaHandler.h"
#include "logevent.h"

LogEvent nodeEventTimer::event = LogEvent(EventType_TIMER);

nodeEventTimer::nodeEventTimer(quint64 startTime, node *eventNode, QString type)
    :nodeEvent(startTime,eventNode)
{
    m_type = type;
}

void nodeEventTimer::action ()
{
    qDebug() << "innerEvent";

    // LogEvent event(EventType_TIMER);
    qDebug() << "0";
    qDebug() << startTime;
    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    qDebug() << "01";
    qDebug() << eventNode->getMAC();

    qDebug() << "1";

    log::writeLog(event);

    eventNode->isTimerEnabled = false;

    qDebug() << "2";
    // событие с которым сейчас работает обработчик lua
    luaHandler::currentNode = eventNode;

    //функция lua, которую нужно вызвать
    lua_getglobal(eventNode->getLua(),
                  eventNode->getNodeType()->functionName(TIMER_INTERRUPT_HANDLER));
    lua_pushstring(eventNode->getLua(), m_type.toLocal8Bit().data());

    qDebug() << "3";
    
    //вызов функции eventAction
    if (lua_pcall(eventNode->getLua(), 1, 0, 0) != 0) 
        std::cerr << "Warning: node " << eventNode->getMAC() << " eventActionInner error "
                  << " at time " << startTime << std::endl;

    qDebug() << "end timer interrupt";
}

bool nodeEventTimer::check()
{
    return true;
}
