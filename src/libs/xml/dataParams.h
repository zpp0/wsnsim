/***********************************************
 *
 * File: dataParams.h
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#ifndef DATAPARAMS_H
#define DATAPARAMS_H

#include <QtCore>
#include <QVector>

// Единицы измерений времени работы симулятора
typedef enum timeUnits {
    us, ms, s, m, h, d, w, months
} simulatorTimeUnits;

typedef struct envParams {
    // СВОЙСТВА - ХАРАКТЕРИСТИКИ СРЕДЫ
    // максимально-возможное время работы симулятора
    quint64 maxSimulatorWorkTime;

    // единицы измерений времени работы симулятора
    simulatorTimeUnits timeUnits;
    
    // размеры среды по 3 измерениям (х,у,z)
    double size[3];
} envParams;

typedef struct projectParams {
    
    // имя автора проекта
    QString autor;

    // название проекта
    QString projectName;

    // комментарий
    QString comment;
    
} projectParams;

typedef struct simulatorParams {

    // версия симулятора
    quint16 version;

} simulatorParams;

typedef struct processParams {

    // ID процесса
    quint16 processID;

    // название процесса
    QString name;

    // единицы измерений
    QString units;

    // комментарий к процессу
    QString comment;

    // файл lua этого процесса
    QString luaScript;

} processParams;

typedef struct sensorParams {

    // ID датчика на узле
    quint16 sensorID;

    // процесс, который измеряет датчик
    quint16 processID;

    // название датчика
    QString name;

    // единицы измерений
    QString units;
    
    // пороговые значения датчика
    double minValue;
    double maxValue;

} sensorParams;

typedef enum freqBand {

    // частотный диапазон радио
    MHz2450, MHz915, MHz868

} freqBand;

typedef struct nodeTypeParams {

    // ID типа узлов
    quint16 nodeTypeID;

    // имя типа
    QString name;

    // мощность передатчика [дБмВт]
    double transmitterPower;
    // чувствительность приемника [дБмВт]
    double receiverSensivity;

    // диапазон частот [МГц]
    freqBand frequency;

    // на узлах установлены направленные антенны?
    bool directionalAntenna;
    
    // массив датчиков
    QVector<sensorParams*> sensors;

    // невозобновляемый источник питания?
    bool unrenewableEnergy;
    // заряд [мАч]
    unsigned int energy;

    // может ли узел перемещаться?
    bool movement;

    // использует ли узел реальную модель таймера?
    bool realClock;

    // список id процессов, от которых зависит таймер
    QList<quint16> clockProcessDependence;
    
    // файлы сценариев lua
    // NOTE: пока тут полный путь, потом сделаю относительный
    // программа
    QString luaFile;

    // скрипт lua, определяющий время включения узла
    QString luaFileStartTime;
    
    QHash<QString, QString> functions;
    
} nodeTypeParams;

typedef struct nodeParams {
    // ID узла
    quint16 nodeID;

    // ID типа узла
    quint16 nodeTypeID;

    // координаты узла
    double coord[3];

    // флаг случайных координат
    bool random;

} nodeParams;

typedef struct dataParams {
    // структура параметров окружения
    envParams* env;

    // структура параметров проекта
    projectParams* project;

    // структура параметров симулятора
    simulatorParams* simulator;
    
    // массив структур параметров процессов
    QVector<processParams*> processes;

    // массив структур параметров типов узлов
    QVector<nodeTypeParams*> nodeTypes;

    // массив структур параметров узлов
    QVector<nodeParams*> nodes;
    
} dataParams;

#endif // DATAPARAMS_H
