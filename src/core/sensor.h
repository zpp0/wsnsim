/***********************************************
 *
 * File: sensor.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef SENSOR_H
#define SENSOR_H

#include "process.h"

#include "dataParams.h"

class sensor
{
public:
    // конструктор класса
    // в качестве аргумента принимает имя измеряемого процесса, координаты, в которых установлен датчик и измеряемый диапазон
    sensor(sensorParams* params, quint16 nodeID);
    // измерение
    double measure();
    
private:
    // ID датчика на узле
    quint16 m_sensorID;

    quint16 m_nodeID;

    QString m_name;
    QString m_units;
    
    // процесс, который измеряет датчик
    process* m_proc;

    // пороговые значения датчика
    double m_minValue;
    double m_maxValue;
};

#endif // SENSOR_H
