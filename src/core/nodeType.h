/***********************************************
 *
 * File: nodeType.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef NODETYPE_H
#define NODETYPE_H

#include <QString>
#include <QVector>

#include "dataParams.h"

class nodeType
{
public:
    
    // конструктор
    // принимает имя типа
    nodeType(nodeTypeParams* params);
    ~nodeType();

    // геттеры
    
    // Имя типа
    const QString name() const { return m_name; }
    // мощность передатчика [дБмВт]
    double transmitterPower() const { return m_transmitterPower; }
    // чувствительность приемника [дБмВт]
    double receiverSensivity() const { return m_receiverSensivity; }
    // диапазон частот [МГц]
    freqBand frequency() const { return m_freqBand; }
    // FIXME: переименовать
    // флаг наличия постоянного источника питания
    bool isUnrenewableEnergy() const { return m_unrenewableEnergy; }
    // может ли узлы передвигаться?
    bool isMovement() const { return m_movement; }
    // использует ли узел реальную модель часов?
    bool isRealClock() const { return m_realClock; }
    // на узлах установлены направленные антенны?
    bool isDirectionalAntenna() const { return m_directionalAntenna; }
    // текущий заряд батареи [мАч]
    unsigned int energy() const { return m_energy; }
    // список параметров всех датчиков
    QVector<sensorParams*>& sersorParamsVector() { return m_sensorsParams; }
    const QString luaFile() { return m_luaFile; }
    const char* functionName(QString function)
    {
        return m_functions[function].toUtf8().constData();
    }
    
    // список ID процессов, от которых зависят часы
    // FIXME: сократить название
    QList<quint16>& clockProcessDependensiesIDList() { return m_clockProcessDependensies; }

private:
    // ID типа узлов
    quint16 m_ID;
    
    // имя типа
    QString m_name;

    // мощность передатчика [дБмВт]
    double m_transmitterPower;
    // чувствительность приемника [дБмВт]
    double m_receiverSensivity;
    // диапазон частот [МГц]
    freqBand m_freqBand;

    // на узлах установлены направленные антенны?
    bool m_directionalAntenna;

    // может ли узел перемещаться?
    bool m_movement;

    // использует ли узел реальную модель таймера?
    bool m_realClock;

    // хеш массив датчиков
    QVector <sensorParams*> m_sensorsParams;

    // список ID процессов, от которых зависят часы
    QList<quint16> m_clockProcessDependensies;
    
    // невозобновляемый источник питания?
    bool m_unrenewableEnergy;
    // заряд [мАч]
    unsigned int m_energy;

    QString m_luaFile;

    QHash<QString, QString> m_functions;
};

#endif // NODETYPE_H
