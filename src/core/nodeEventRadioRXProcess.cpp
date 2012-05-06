/***********************************************
 *
 * File: nodeEventRadioRXProcess.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "nodeEventRadioRXProcess.h"
#include "log.h"
#include "logevent.h"

nodeEventRadioRXProcess::nodeEventRadioRXProcess(quint64 startTime, node *eventNode)
    :nodeEvent(startTime,eventNode)
{
}

void nodeEventRadioRXProcess::action()
{
    qDebug() << "radioRXProcessEvent";

    LogEvent event(EventType_RXProcess);
    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    
    log::writeLog(event);
}
