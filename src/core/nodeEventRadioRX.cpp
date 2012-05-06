/***********************************************
 *
 * File: nodeEventRadioRX.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include "api.h"
#include "nodeEventRadioTX.h"
#include "nodeEventRadioRX.h"
#include "log.h"
#include "channel.h"
#include "logevent.h"

quint64 nodeEventRadioRX::CollCount = 0;
quint64 nodeEventRadioRX::RXCount = 0;

LogEvent nodeEventRadioRX::event = LogEvent(EventType_RX);

nodeEventRadioRX::nodeEventRadioRX(quint64 startTime, node *eventNode, message *mes, double rssi)
    :nodeEvent(startTime,eventNode)
{
    m_mes = mes;
    m_rssi = rssi;
}

bool nodeEventRadioRX::check()
{ 
    if (eventNode->phyMesListen(m_mes) == true) {
        RXCount++;
        return true;
    }


    // std::cerr << "RX count mes" << m_mes << " " << m_mes->RXCount;
    if (--m_mes->RXCount == 0) {
        // std::cerr << "deleting mes " << m_mes << std::endl;
        delete m_mes;
    }
    // env::setMainTime(startTime);


    CollCount++;
    return false;    
}

void nodeEventRadioRX::action()
{
    qDebug() << "radioRXEvent";

    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    Message mes;
    mes.setBody(m_mes->getMessage());
    event.m_prxEventParam->message = mes;
    event.m_prxEventParam->RSSI = m_rssi;

    log::writeLog(event);

    //событие с котрым сейчас работает обработчик lua
    luaHandler::currentNode = eventNode;
    
    //функция lua, которую нужно вызвать
    lua_getglobal(eventNode->getLua(),
                  eventNode->getNodeType()->functionName(RADIO_INTERRUPT_HANDLER));

    //кладём в стек тело сообщения, как таблицу
    lua_newtable(eventNode->getLua());

    // передаем тело сообщения на lua
    for (int i = 0; i < m_mes->getBody().size(); i++) {
         lua_pushnumber(eventNode->getLua(), i + 1);
         lua_pushnumber(eventNode->getLua(), m_mes->getBody().at(i));
         lua_settable(eventNode->getLua(), -3);
    }
    
    //вызов функции eventAction
    if (lua_pcall(eventNode->getLua(), 1, 0, 0) != 0)
        std::cerr << "Warning: node " << eventNode->getMAC() << " eventActionRadio error "
                  << " at time " << startTime << std::endl;

    // std::cerr << "RX count mes" << m_mes << " " << m_mes->RXCount;
    if (--m_mes->RXCount == 0) {
        // std::cerr << "deleting mes" << m_mes;
        delete m_mes;
    }
    // env::setMainTime(startTime);
}
