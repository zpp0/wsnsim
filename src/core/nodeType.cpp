/***********************************************
 *
 * File: nodeType.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include "nodeType.h"
#include "luaHandler.h"
#include "api.h"

nodeType::nodeType (nodeTypeParams* params)
{
    // получаем ID типа узлов
    m_ID = params->nodeTypeID;

    m_name = params->name;

    m_transmitterPower = params->transmitterPower;
    m_receiverSensivity = params->receiverSensivity;
    m_freqBand = params->frequency;
    
    m_unrenewableEnergy = params->unrenewableEnergy;
    m_energy = params->energy;
    m_directionalAntenna = params->directionalAntenna;
    m_movement = params->movement;
    m_realClock = params->realClock;
    m_clockProcessDependensies = params->clockProcessDependence;
    
    m_sensorsParams = params->sensors;

    m_luaFile = params->luaFile;

    QList<QString> stdFunctions;
    stdFunctions.append(NODE_POWERON_TIME);
    stdFunctions.append(NODE_INITIALISATION);
    stdFunctions.append(TIMER_INTERRUPT_HANDLER);
    stdFunctions.append(RADIO_INTERRUPT_HANDLER);

    for (int i = 0; i < stdFunctions.size(); i++)
        m_functions[stdFunctions[i]] = params->functions[stdFunctions[i]];
    
    if (m_movement == true)
        m_functions[NODE_MOVEMENT] = params->functions[NODE_MOVEMENT];
    if (m_realClock == true)
        m_functions[NODE_REAL_TIMER] = params->functions[NODE_REAL_TIMER];
    if (m_directionalAntenna == true)
        m_functions[NODE_DIRECTIONAL_ANTENNA] = params->functions[NODE_DIRECTIONAL_ANTENNA];

}

nodeType::~nodeType ()
{
    for (int i = 0; i < m_sensorsParams.size(); i++)
        delete (m_sensorsParams[i]);
}
