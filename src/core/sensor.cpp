/***********************************************
 *
 * File: sensor.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QString>

#include <iostream>

#include "sensor.h"
#include "env.h"
#include "node.h"

#include "nodeEventMeasure.h"

#include "dataParams.h"

sensor::sensor(sensorParams* params, node* node)
{
    // ID датчика на узле
    m_sensorID = params->sensorID;
    // название датчика
    m_name = params->name;
    // единицы измерений датчика
    m_units = params->units;
    // привязываем датчик к узлу
    m_node = node;
    // получаем процесс по его ID
    process* proc = env::getProc(params->processID);
    if (proc == NULL) {
        std::cerr << "Error: sensor " << m_sensorID << " on node " << m_node->getMAC()
                  << " request output process with ID " << params->processID << std::endl
                  << "Exit." << std::endl;
        exit(1);
    }
    m_proc = proc;
    // верхняя граница чувствительности
    m_minValue = params->minValue;
    // нижняя граница чувствительности
    m_maxValue = params->maxValue;
}

double sensor::measure()
{
    // проводим измерение
    double currentMeasure = m_proc->measure(env::getNodeCoord(m_node), env::getMainTime());
    
    // если измеренное значение выходит за пределы допустимых для датчика
    if (currentMeasure > m_maxValue || currentMeasure < m_minValue) {
        
        // при включенном режиме отладки датчик будет ругаться
        std::cerr << "Warning: sensor " << m_name.toStdString() << " measure value " <<  currentMeasure << " " << m_units.toStdString()
                  << " is out of measure range: [ " << m_minValue << " : " << m_maxValue << " ] "
                  << " at time " << env::getMainTime()
                  << " on node " << m_node->getMAC() << std::endl;

        // TODO: тут надо подумать
        // и возвращаем 0
        return 0;
    }

    qDebug() << "create measure event";
    
    nodeEventMeasure* event = new nodeEventMeasure(env::getMainTime(), m_node, m_sensorID, currentMeasure);

    event->action();
    // env::insertEventQueue(event);

    return currentMeasure;
}
