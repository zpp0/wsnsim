/***********************************************
 *
 * File: nodeEventOn.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <iostream>

#include <QString>
#include <QVector>

#include "api.h"
#include "nodeEventOn.h"
#include "log.h"
#include "luaHandler.h"
#include "logevent.h"

nodeEventOn::nodeEventOn(quint64 startTime, node *eventNode, double* coords)
    :nodeEvent(startTime,eventNode)
{
    m_coords = coords;
}

void nodeEventOn::action()
{
    qDebug() << "node created at time" << startTime;

    // std::cerr << "new node " << eventNode << std::endl;
    // std::cerr << "new node id " << eventNode->getMAC() << std::endl;
    
    LogEvent event(EventType_NodeOn);

    event.m_time = startTime;
    event.m_nodeId = eventNode->getMAC();
    event.m_pNodeOnParam->x = m_coords[0];
    event.m_pNodeOnParam->y = m_coords[1];
    event.m_pNodeOnParam->z = m_coords[2];

    log::writeLog(event);

    // регистрируем узел в среде
    env::registerNode(eventNode, m_coords);
    
    env::nodesHearTest();

    // инициализация оборудования на узлах
    luaHandler::currentNode = eventNode;

    lua_State* luaVM = luaHandler::currentNode->getLua();
    // выполняем инициализацию узлов
    lua_getglobal(luaVM, eventNode->getNodeType()->functionName(NODE_INITIALISATION));
    
    // вызов функции инициализации узла
    if (lua_pcall(luaVM, 0, 0, 0) != 0)
        std::cerr << "Warning: Node " << eventNode->getMAC() << " initializeNode error " << std::endl;

    // std::cerr << "asd";
}
