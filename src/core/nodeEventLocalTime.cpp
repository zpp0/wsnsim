/***********************************************
 *
 * File: nodeEventLocalTime.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "nodeEventLocalTime.h"
#include "log.h"
#include "logevent.h"

nodeEventLocalTime::nodeEventLocalTime(quint64 startTime, node *eventNode, quint64 localTime)
    :nodeEvent(startTime,eventNode)
{
    m_localTime = localTime;
}

void nodeEventLocalTime::action()
{
    // пишем его в лог
    LogEvent event(EventType_LOGTIME);
    event.m_time = env::getMainTime();
    event.m_nodeId = eventNode->getMAC();
    event.m_plogtimeEventParam->localTime = m_localTime;
    
    log::writeLog(event);
}

bool nodeEventLocalTime::check()
{
    return true;
}
