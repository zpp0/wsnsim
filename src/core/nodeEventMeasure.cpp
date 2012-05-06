/***********************************************
 *
 * File: nodeEventMeasure.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/


#include <QString>
#include <QVector>

#include "nodeEventMeasure.h"
#include "log.h"
#include "luaHandler.h"
#include "logevent.h"

nodeEventMeasure::nodeEventMeasure(quint64 startTime, node* eventNode, quint16 sensorID, double value)
    :nodeEvent(startTime,eventNode)
{
    m_sensorID = sensorID;

    m_value = value;
}

void nodeEventMeasure::action()
{
    qDebug() << "event measure";
        
    LogEvent event(EventType_MEASURE);

    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    event.m_pmeasureEventParam->sensorId = m_sensorID;
    event.m_pmeasureEventParam->value = m_value;

    log::writeLog(event);
}
