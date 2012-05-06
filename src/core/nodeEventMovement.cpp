/***********************************************
 *
 * File: nodeEventMovement.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QString>
#include <QVector>

#include "nodeEventMovement.h"
#include "log.h"
#include "luaHandler.h"
#include "logevent.h"

nodeEventMovement::nodeEventMovement(quint64 startTime, node *eventNode, double newCoord[3])
    :nodeEvent(startTime,eventNode)
{
    m_coord = newCoord;
}

bool nodeEventMovement::check()
{
    return true;
}

void nodeEventMovement::action()
{
    qDebug() << "moveEvent";
    
    LogEvent event(EventType_MOVE);
    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    event.m_pmoveEventParam->x = m_coord[0];
    event.m_pmoveEventParam->y = m_coord[1];
    event.m_pmoveEventParam->z = m_coord[2];
    
    log::writeLog(event);
}
