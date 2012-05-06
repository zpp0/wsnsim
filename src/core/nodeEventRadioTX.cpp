/***********************************************
 *
 * File: nodeEventRadioTX.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "nodeEventRadioTX.h"
#include "log.h"
#include "logevent.h"

LogEvent nodeEventRadioTX::event = LogEvent(EventType_TX);

nodeEventRadioTX::nodeEventRadioTX(quint64 startTime, node *eventNode, message* mes, double rssi)
    :nodeEvent(startTime,eventNode)
{
    m_mes = mes;
    m_rssi = rssi;
}

void nodeEventRadioTX::action()
{
    qDebug() << "txEvent";

    event.m_time = startTime; 
    event.m_nodeId = eventNode->getMAC();
    Message mes;
    mes.setBody(m_mes->getMessage());
    event.m_ptxEventParam->message = mes;
    event.m_ptxEventParam->RSSI = m_rssi;

    log::writeLog(event);
}



