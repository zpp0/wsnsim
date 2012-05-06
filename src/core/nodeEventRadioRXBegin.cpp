/***********************************************
 *
 * File: nodeEventRadioRXBegin.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "nodeEventRadioRXBegin.h"
#include "nodeEventRadioRX.h"
#include "log.h"

nodeEventRadioRXBegin::nodeEventRadioRXBegin(quint64 startTime, node *eventNode, message *mes, double rssi)
    :nodeEvent(startTime,eventNode)
{
    m_mes = mes;
    m_rssi = rssi;
}

void nodeEventRadioRXBegin::action()
{
    qDebug() << "radioRXEvent";

    if (eventNode->phyMesListen(m_mes) == false)
        return;

    nodeEventRadioRX* radioEvent;
    radioEvent = new nodeEventRadioRX(startTime + m_mes->getTransmitTime(),
                                      eventNode,
                                      m_mes,
                                      m_rssi);
    env::insertEventQueue(radioEvent);

    env::setMainTime(startTime + m_mes->getTransmitTime());
}

bool nodeEventRadioRXBegin::check()
{
    return true;
}
