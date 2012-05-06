/***********************************************
 *
 * File: simulator.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <iostream>
#include <cassert>

#include "simulator.h"
#include "env.h"
#include "node.h"
#include "nodeEventRadioRX.h"
#include "nodeEvent.h"
#include "luaHandler.h"
#include "log.h"
#include "event.h"

#include "xml.h"
#include "dataParams.h"

simulator::simulator(QString projectFileName, log::LogFormat format)
{
    QFileInfo info = QFileInfo(QDir::current(), projectFileName);
    info.setFile(info.absolutePath());

    if (info.exists() && info.isDir())
        m_currentProjectPath = info.absoluteFilePath();
    else
        qFatal("ERROR: There is no directory in argv. Exit programm");

    qDebug() << "Project path is" << m_currentProjectPath;
    
    log::init(m_currentProjectPath + "/", format);
    luaHandler::currentProjectPath = m_currentProjectPath;

    loadProject(projectFileName);
}

simulator::~simulator()
{
    log::uninit();
}

void simulator::loadProject (QString projectFileName)
{
    qDebug() << "read xml file";
    
    // получаем данные из xml
    dataParams* params = loadDataParams(projectFileName);

    if (params == NULL) {
        std::cerr << "Error: cannot load xml file " << projectFileName.toStdString() << std::endl
                  << "Exit." << std::endl;
        exit(1);
    }
    
    // сравниваем версию симулятора с версией данных
    if (params->simulator->version != m_version)
        std::cerr << "Warning: Version of core and xml data are differens" << std::endl;

    // устанавливаем параметры на страницах
    // параметры среды
    env::setEnv(params->env);
    // параметры процессов
    env::setProcesses(params->processes);
    // паарметры типов узлов
    env::setNodeTypes(params->nodeTypes);
    // параметры узлов
    env::setNodes(params->nodes);

    qDebug() << "Loaded params:"
             << "\nENV"
             << "\nMax time" << params->env->maxSimulatorWorkTime
             << "\nTime units" << params->env->timeUnits
             << "\nsize" << params->env->size[0] << params->env->size[1] << params->env->size[2]
             << "\n"
             << params->processes.size() << "processes"
             << params->nodeTypes.size() << "nodeTypes"
             << params->nodes.size() << "nodes";
    
    for (int i = 0; i < params->processes.size(); i++) {
        qDebug() << "PROCESSES"
                 << "\nid" << params->processes[i]->processID
                 << "\nname" << params->processes[i]->name
                 << "\nunits" << params->processes[i]->units
                 << "\nlua" << params->processes[i]->luaScript;

        delete params->processes[i];
    }
    for (int i = 0; i < params->nodeTypes.size(); i++) {
        qDebug() << "\nNODETYPES"
                 << "\nid" << params->nodeTypes[i]->nodeTypeID
                 << "\nname" << params->nodeTypes[i]->name
                 << "\ntransmitterPower" << params->nodeTypes[i]->transmitterPower
                 << "\nreceiverSensivity" << params->nodeTypes[i]->receiverSensivity
                 << "\nfrequency" << params->nodeTypes[i]->frequency
                 << "\ndirectionalAntenna" << params->nodeTypes[i]->directionalAntenna
                 << "\nunrenewableEnergy" << params->nodeTypes[i]->unrenewableEnergy
                 << "\nenergy" << params->nodeTypes[i]->energy
                 << "\nmovement" << params->nodeTypes[i]->movement
                 << "\nrealClock" << params->nodeTypes[i]->realClock;
        
        for (int j = 0; j < params->nodeTypes[i]->sensors.size(); j++) {

            qDebug() << "\nSENSORS"
                     << "\nsensorID" << params->nodeTypes[i]->sensors[j]->sensorID
                     << "\nprocessID" << params->nodeTypes[i]->sensors[j]->processID
                     << "\nname" << params->nodeTypes[i]->sensors[j]->name
                     << "\nunits" << params->nodeTypes[i]->sensors[j]->units
                     << "\nminValue" << params->nodeTypes[i]->sensors[j]->minValue
                     << "\nmaxValue" << params->nodeTypes[i]->sensors[j]->maxValue;
            
            delete params->nodeTypes[i]->sensors[j];
        }

        for (int j = 0; j < params->nodeTypes[i]->clockProcessDependence.size(); j++) {

            qDebug() << "\nPROCESSDEPENDS"
                     << "\nID" << params->nodeTypes[i]->clockProcessDependence[j];
            
        }
        
        delete params->nodeTypes[i];
    }
    
    for (int i = 0; i < params->nodes.size(); i++) {

        qDebug() << "\nNODES"
                 << "\nnodeID" << params->nodes[i]->nodeID
                 << "\nnodeTypeID" << params->nodes[i]->nodeTypeID
                 << "\ncoord" << params->nodes[i]->coord[0] << params->nodes[i]->coord[1] << params->nodes[i]->coord[2]
                 << "\nrandom" << params->nodes[i]->random;

        
        delete params->nodes[i];
    }

    delete params->env;
    delete params->project;
    delete params->simulator;
    delete params;
}

void simulator::eval()
{
    qDebug() << "max time" << env::getMaxSimulatorWorkTime();
    
    quint64 oldTime = 0;

    int completionPersent = 0;
    
    event* nextEvent;

    // node* node = NULL;

    // для всех событий из очереди
    // извлекаем событие и отправляем соответствующее сообщение
    while ((nextEvent = env::popEventQueue()) != NULL) {

        // qDebug() << "time" << env::getMainTime() << nextEvent->startTime;
        
        // если текущее время равно или превышает максимальное
        if (nextEvent->startTime >= env::getMaxSimulatorWorkTime()) {
            if (completionPersent < 100) {
                completionPersent = 100;
                std::cout << "\r" << std::unitbuf << completionPersent << "%";
            }
            delete nextEvent;
            break;
        }

        oldTime = env::getMainTime();

        // if ((nextEvent->startTime >= oldTime)
        //     || (node != ((nodeEvent*)nextEvent)->eventNode))
        // {
        if (nextEvent->check() == true) {
            //устанавливаем время, на время начала события
            env::setMainTime(nextEvent->startTime);

            if (oldTime != env::getMainTime()) {
                if (env::moveNodes())
                    env::nodesHearTest();
            }

            // выполнения события
            nextEvent->action();
        }

        // node = ((nodeEvent*)nextEvent)->eventNode;

        qint64 remainingTime = env::getMaxSimulatorWorkTime() - nextEvent->startTime;
        int currentPercent = ((env::getMaxSimulatorWorkTime() - remainingTime) * 100) / env::getMaxSimulatorWorkTime();
        if (completionPersent != currentPercent) {
            completionPersent = currentPercent;
            std::cout << "\r" << std::unitbuf << completionPersent << "%";
        }
        
        delete nextEvent;
    }

    std::cerr << std::endl << event::Count << " events" << std::endl;
    std::cerr << std::endl << nodeEventRadioRX::CollCount << " collisions" << std::endl;
    std::cerr << std::endl << nodeEventRadioRX::RXCount << " RX messages" << std::endl;
    // qDebug() << event::Count << "events";
}
